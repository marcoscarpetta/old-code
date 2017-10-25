#
# Copyright (C) 2017 Marco Scarpetta
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import os
import ruamel.yaml as yaml
from datetime import datetime, timedelta
import unicodedata
import config
import shutil
import pystache
from posixpath import join as urljoin
import hashlib
import io
import zipfile
import bleach

def str_presenter(dumper, value):
    if "\n" in value:
        value = value.replace("\r\n", "\n")
        return dumper.represent_scalar('tag:yaml.org,2002:str', str(value), style="|")
    return dumper.represent_scalar('tag:yaml.org,2002:str', value, "\"")

yaml.add_representer(str, str_presenter)

def pretty_title(title: str, words_number: int=5) -> str:
    title = unicodedata.normalize('NFKD', title)
    title = title.lower()
    allowed = "abcdefghijklmnopqrstuvwxyz0123456789 "
    title = "".join(l for l in title if l in allowed)
    
    words = [word for word in title.split(" ") if word != ""]
    
    if len(words) == 0:
        return "untitled"
    
    pretty_title = ""
    for word in words:
        pretty_title += word + "-"
    
    return pretty_title[:-1]

def render_template(template_file: str, **data) -> str:
    for url in config.url:
        data[url] = config.url[url]
        
    f = open(os.path.join(config.templates_dir, template_file), "r", encoding='utf-8')
    string = f.read()
    f.close()
    
    return pystache.render(string, data)

def load_yaml_file(file_path: str) -> dict:
    f = open(file_path, "r", encoding='utf-8')
    data = yaml.safe_load(f.read())
    f.close()
    
    return data

def save_yaml_file(file_path: str, data: dict) -> None:
    f = open(file_path, "w", encoding='utf-8')
    f.write(yaml.dump(data, default_flow_style=False, indent=4, block_seq_indent=2))
    f.close()

class User():
    def __init__(self, username: str=None) -> None:
        data = {}
        
        if username:            
            user_file_path = os.path.join(config.users_dir, username + ".yaml")

            if os.path.isfile(user_file_path):
                data = load_yaml_file(user_file_path)
                if data == None:
                    data = {}

        self._oauth2_id = data.get("oauth2_id", "")
        self.session_id = data.get("session_id", "")
        self.level = data.get("level", 10)
        self.username = data.get("username", "")
        self.email = data.get("email", "")
        self._name = data.get("name", "")
        self._picture = data.get("picture", "")
        self.bio = data.get("bio", "")
        self.blocked = data.get("blocked", False)
        self.hide_content = data.get("hide_content", False)
        self.hide_picture = data.get("hide_picture", False)

    @property
    def oauth2_id(self) -> str:
        return self._oauth2_id

    @oauth2_id.setter
    def oauth2_id(self, value: str) -> None:
        self._oauth2_id = value
        if "{}@{}".format(config.root_user_id, config.root_user_provider) == self._oauth2_id:
            self.level = 0

    @property
    def name(self) -> str:
        return self._name

    @name.setter
    def name(self, value: str) -> None:
        self._name = value
        
        #if this is a new user, assign username
        if len(self.oauth2_id) != 0 and len(self.username) == 0:
            usernames = load_yaml_file(config.usernames_file)
            
            if not usernames:
                usernames = {}
            
            if self.oauth2_id not in usernames:
                username = pretty_title(self._name)
            
                if username in usernames.values():
                    n = 0
                    while "{}-{}".format(username, n) in usernames.values():
                        n += 1
                    self.username = "{}-{}".format(username, n)
                else:
                    self.username = username
                
                usernames[self.oauth2_id] = self.username
                
                save_yaml_file(config.usernames_file, usernames)
            else:
                self.username = usernames[self.oauth2_id]
    
    @property
    def picture(self) -> str:
        if self.hide_picture:
            return urljoin(config.url["static_url"], "picture.svg")
        else:
            return self._picture
    
    @picture.setter
    def picture(self, value: str) -> None:
        self._picture = value

    def save(self) -> None:
        data = {
            "oauth2_id": self.oauth2_id,
            "session_id": self.session_id,
            "level": self.level,
            "username": self.username,
            "email": self.email,
            "name": self.name,
            "picture": self._picture,
            "bio": self.bio,
            "blocked": self.blocked,
            "hide_content": self.hide_content,
            "hide_picture": self.hide_picture
        }
        
        save_yaml_file(os.path.join(config.users_dir, self.username + ".yaml"), data)
    
    def update_session_id(self, response) -> str:
        self.session_id = hashlib.sha256(os.urandom(1024)).hexdigest()
        self.save()
        
        response.set_cookie(
            "session_id",
            value = self.username + "=" + self.session_id,
            expires = datetime.utcnow() + timedelta(days=30),
            secure = config.secure_cookies,
            httponly = True
        )

def get_user_by_oauth2_id(oauth2_id: str) -> User:
    usernames = load_yaml_file(config.usernames_file)
    
    if usernames != None and oauth2_id in usernames:
        return User(usernames[oauth2_id])
    
    return None

def get_logged_user(cookies: dict) -> User:
    if cookies.get("session_id"):
        user_session_id = cookies.get("session_id").split("=")
        if len(user_session_id) == 2 and len(user_session_id[1]) == 64:
            user = User(user_session_id[0])
            if user.session_id == user_session_id[1]:
                return user
    
    return None

def get_users_list() -> list:
    users = []
    
    for filename in os.listdir(config.users_dir):
        user = User(filename.replace(".yaml", ""))
        if user:
            users.append(user)
    
    return users

class Post():
    def __init__(self, post_id: str=None) -> None:
        self.is_new_post = True
        data = {}
        
        if post_id:
            index_file_path = os.path.join(config.posts_dir, post_id, "index.yaml")

            if os.path.isfile(index_file_path):
                self.is_new_post = False
                data = load_yaml_file(index_file_path)
        
        self.id = post_id or ""
        
        if "date" in data:
            self._date = datetime.strptime(data.get("date", ""), config.date_format)
        else:
            self._date = datetime.utcnow()
        
        if "edit_date" in data:
            self._edit_date = datetime.strptime(data.get("edit_date", ""), config.date_format)
        else:
            self._edit_date = datetime.utcfromtimestamp(0)
        
        self._title = data.get("title", "")
        self.edit_body = data.get("body", "")
        self.tags = data.get("tags", [])
        self.list_post = data.get("list_post", True)
        self._draft = data.get("draft", False)
        self.allow_comments = data.get("allow_comments", True)
        self.author = data.get("author", "")
        
        self.logged_user = None
    
    @property
    def title(self) -> str:
        return self._title

    @title.setter
    def title(self, value: str) -> None:
        self._title = value
        
        #if this is a new post, assign id
        if len(self.id) == 0:
            post_id = pretty_title(self._title)
        
            if os.path.isdir(os.path.join(config.posts_dir, post_id)):
                n = 0
                while os.path.isdir(os.path.join(config.posts_dir, post_id + "-{}".format(n))):
                    n += 1
                self.id = post_id + "-{}".format(n)
            else:
                self.id = post_id
            
            os.mkdir(os.path.join(config.posts_dir, self.id))
    
    @property
    def draft(self) -> bool:
        return self._draft

    @draft.setter
    def draft(self, value: bool) -> None:
        if self._draft == False and value == True:
            self.is_new_post = True
        
        self._draft = value
    
    @property
    def author(self) -> str:
        return self._author.username

    @author.setter
    def author(self, value: str) -> None:
        self._author = User(value)
    
    @property
    def author_name(self) -> str:
        return self._author.name
    
    @property
    def body(self) -> str:
        return self.edit_body.replace(
            "{{post_static}}",
            urljoin(config.url["static_url"], self.id)
        )
    
    @property
    def summary(self) -> str:
        stop = self.body.find("</p>")
        if stop > -1:
            return self.body[0:stop + 4]
        
        return self.body
    
    @property
    def date(self) -> str:
        return self._date.strftime(config.date_format)
    
    @property
    def display_date(self) -> str:
        return self._date.strftime(config.display_date_format)
    
    @property
    def edit_date(self) -> str:
        if self._edit_date > self._date:
            return self._edit_date.strftime(config.date_format)
        
        return ""
    
    @property
    def display_edit_date(self) -> str:
        if self._edit_date > self._date:
            return self._edit_date.strftime(config.display_date_format)
        
        return ""
    
    @property
    def static_files(self) -> list:
        files = []
        for f in os.listdir(os.path.join(config.posts_dir, self.id)):
            if os.path.isfile(os.path.join(config.posts_dir, self.id, f)) and f != "index.yaml" and f != "comments.yaml":
                files.append(f)
        
        return files
    
    @property
    def raw_comments(self) -> list:
        if not os.path.isfile(os.path.join(config.posts_dir, self.id, "comments.yaml")):
            return []
        
        return load_yaml_file(os.path.join(config.posts_dir, self.id, "comments.yaml"))
    
    @property
    def comments(self) -> list:
        comments = self.raw_comments
        
        for i in range(len(comments)):
            comments[i]["author"] = User(comments[i]["author"])
            comments[i]["date"] = datetime.strptime(comments[i]["date"], config.date_format).strftime(config.display_date_format)
            comments[i]["delete_comment_id"] = None
            if self.logged_user == comments[i]["author"].username:
                comments[i]["delete_comment_id"] = self.id + "/" + str(i)
        
        return comments
    
    @property
    def comments_count(self) -> int:
        return len(self.raw_comments)
    
    def delete(self) -> None:
        absolute_post_dir_path = os.path.join(config.posts_dir, self.id)
        
        if os.path.isdir(absolute_post_dir_path):
            shutil.rmtree(absolute_post_dir_path)

    def delete_static_file(self, filename: str) -> None:
        if os.path.isfile(os.path.join(config.posts_dir, self.id, filename)):
            os.remove(os.path.join(config.posts_dir, self.id, filename))
                
    def save_static_file(self, file) -> None:
        file.filename = file.filename.replace("/", "")
        file.save(os.path.join(config.posts_dir, self.id, file.filename))

    def save(self) -> None:
        #ensure that this post has a folder
        if len(self.id) == 0:
            self.title = self._title
        
        if self.is_new_post:
            self._date = datetime.utcnow()
            self._edit_date = datetime.utcfromtimestamp(0)
        else:
            self._edit_date = datetime.utcnow()
        
        self.is_new_post = False
        
        post = {
            "title": self._title,
            "body": self.edit_body,
            "tags": self.tags,
            "author": self.author,
            "list_post": self.list_post,
            "draft": self.draft,
            "allow_comments": self.allow_comments,
            "date": self.date,
            "edit_date": self._edit_date.strftime(config.date_format)
        }
        
        save_yaml_file(os.path.join(config.posts_dir, self.id, "index.yaml"), post)
    
    def add_comment(self, user: User, comment: str) -> None:
        comments = self.raw_comments
        
        comments.append({
            "body": bleach.clean(
                comment,
                tags=['a', 'b', 'i'],
                attributes={'a': ['href', 'title']}
            ),
            "author": user.username,
            "date": datetime.utcnow().strftime(config.date_format),
            "deleted": False
        })
        
        save_yaml_file(os.path.join(config.posts_dir, self.id, "comments.yaml"), comments)

    def delete_comment(self, user: User, comment_id: int) -> None:
        comments = self.raw_comments
        
        if comments[comment_id]["author"] == user.username:
            comments[comment_id]["deleted"] = True
        
        save_yaml_file(os.path.join(config.posts_dir, self.id, "comments.yaml"), comments)

def get_posts_list() -> list:
    posts = []
    
    for post_dir in os.listdir(config.posts_dir):
        post = Post(post_dir)
        if len(post.id) > 0:
            posts.append(post)
    
    return posts

def query(posts: list, *args) -> list:
    if len(args)%3 != 0:
        raise Exception("ERROR: invalid number of arguments")
    
    j = len(posts) - 1
    while j >= 0:
        for i in range(int(len(args)/3)):
            #equal query
            if args[i*3+1] == "equal":
                if getattr(posts[j], args[i*3]) != args[i*3+2]:
                    posts.pop(j)
                    break
            
            #lt query
            if args[i*3+1] == "lt":
                if getattr(posts[j], args[i*3]) >= args[i*3+2]:
                    posts.pop(j)
                    break
            
            #gt query
            if args[i*3+1] == "gt":
                if getattr(posts[j], args[i*3]) <= args[i*3+2]:
                    posts.pop(j)
                    break
            
            #contains queries
            if args[i*3+1] == "contains":
                if isinstance(getattr(posts[j], args[i*3]), str) or isinstance(getattr(posts[j], args[i*3]), list):
                    if args[i*3+2] not in getattr(posts[j], args[i*3]):
                        posts.pop(j)
                        break
                else:
                    print("WARNING: contains query on", getattr(posts[j], args[i*3]), " not executed")
        j -= 1
        
    return posts

def sort(posts: list, key: str, descending: bool) -> list:
    i = 0
    while i < len(posts):
        min_max = i
        j = i
        while j < len(posts):
            if (getattr(posts[j], key) > getattr(posts[min_max], key)) == descending:
                min_max = j
            j += 1

        if min_max != i:
            tmp = posts[i]
            posts[i] = posts[min_max]
            posts[min_max] = tmp
        i += 1

    return posts

def backup() -> io.BytesIO:
    f = io.BytesIO()
    z_f = zipfile.ZipFile(f, 'w')
    
    #posts
    for post in get_posts_list():
        z_f.write(
            os.path.join(config.posts_dir, post.id, "index.yaml"),
            os.path.join("/posts/", post.id, "index.yaml")
        )
        for static_file in post.static_files:
            z_f.write(
                os.path.join(config.posts_dir, post.id, static_file),
                os.path.join("/posts/", post.id, static_file)
            )
        if os.path.isfile(os.path.join(config.posts_dir, post.id, "comments.yaml")):
            z_f.write(
                os.path.join(config.posts_dir, post.id, "comments.yaml"),
                os.path.join("/posts/", post.id, "comments.yaml")
            )
    
    #users
    for user in get_users_list():
        z_f.write(
            os.path.join(config.users_dir, user.username + ".yaml"),
            os.path.join("/users/", user.username + ".yaml")
        )
    
    #usernames file
    z_f.write(config.usernames_file, "/usernames.yaml")
    z_f.close()
    
    f.seek(0)
    
    return f

def restore_backup(f) -> bool:
    z_f = zipfile.ZipFile(f)
    
    #check zip file
    files = z_f.namelist()
    if len(files) == 0:
        return False #refuse void zip files
    
    for file_name in files:
        if file_name == "usernames.yaml" or file_name.startswith("posts/") or file_name.startswith("users/"):
            pass
        else:
            return False
    
    #delete everything
    shutil.rmtree(config.posts_dir)
    shutil.rmtree(config.users_dir)
    os.remove(config.usernames_file)
    
    #init directories
    os.mkdir(config.posts_dir)
    os.mkdir(config.users_dir)
    open(config.usernames_file, "w").close()
    
    #restore backup
    for file_name in files:
        if file_name.startswith("posts/"):
            post_id, post_file = os.path.split(file_name.replace("posts/", ""))
            
            post_dir = os.path.join(config.posts_dir, post_id)
            
            if not os.path.isdir(post_dir):
                os.mkdir(post_dir)
            
            extracted = open(os.path.join(post_dir, post_file), "wb")
            shutil.copyfileobj(z_f.open(file_name, "r"), extracted)
            extracted.close()
        
        elif file_name.startswith("users/"):
            extracted = open(os.path.join(config.users_dir, os.path.split(file_name)[1]), "wb")
            shutil.copyfileobj(z_f.open(file_name, "r"), extracted)
            extracted.close()
        
        elif file_name == "usernames.yaml":
            extracted = open(config.usernames_file, "wb")
            shutil.copyfileobj(z_f.open(file_name, "r"), extracted)
            extracted.close()
            
    return True
