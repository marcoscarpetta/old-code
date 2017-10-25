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
from flask import Flask, send_from_directory, request, redirect, make_response, send_file
import config
import blog_backend
from posixpath import join as urljoin
from datetime import datetime, timedelta
import hashlib
import requests
import re
import json

app = Flask(__name__)
app.config['DEBUG'] = True

@app.route(config.url["index_url"], strict_slashes=False)
@app.route(urljoin(config.url["page_url"], "<page>"), strict_slashes=False)
@app.route(urljoin(config.url["tag_url"], "<tag>"), strict_slashes=False)
@app.route(urljoin(config.url["tag_url"], "<tag>", "<page>"), strict_slashes=False)
@app.route(urljoin(config.url["author_url"], "<author>"), strict_slashes=False)
@app.route(urljoin(config.url["author_url"], "<author>", "<page>"), strict_slashes=False)
def posts_list(page=0, tag=None, author=None):
    page = int(page)
    
    posts = blog_backend.sort(
        blog_backend.query(blog_backend.get_posts_list(), "draft", "equal", False),
        "date",
        True
    )
    
    title = None
    page_url = ""
    
    if tag:
        page_url = urljoin(config.url["tag_url"], tag)
        posts = blog_backend.query(posts, "tags", "contains", tag)
        if page > 0:
            title = 'Tag "{}" - page {}'.format(tag, page)
        else:
            title = 'Tag "{}"'.format(tag)
    elif author:
        page_url = urljoin(config.url["author_url"], author)
        author = blog_backend.User(author)
        posts = blog_backend.query(posts, "author", "equal", author.username)
        if page > 0:
            title = 'Author: {} - page {}'.format(author.name, page)
        else:
            title = 'Author: {}'.format(author.name)
    else:
        page_url = config.url["page_url"] 
        posts = blog_backend.query(posts, "list_post", "equal", True)
        if page > 0:
            title = 'Page {}'.format(page)
        else:
            title = "Home Page"
    
    prev_page = urljoin(page_url, str(page - 1))
    next_page = urljoin(page_url, str(page + 1))
    
    if (page == 0):
        prev_page = None
        
    if (len(posts) <= (page + 1)*config.posts_per_page):
        next_page = None
    
    i = page*config.posts_per_page
    
    if i >= len(posts):
        posts = []
        prev_page = None
        next_page = None
    else:
        posts = posts[i:i+config.posts_per_page]
    
    return blog_backend.render_template(
        "index.html",
        content = blog_backend.render_template(
            "page.html",
            next_page_url=next_page,
            prev_page_url=prev_page,
            tag=tag,
            author=author,
            posts_list=posts
        ),
        title=title
    )

@app.route(urljoin(config.url["post_url"], '<post_id>'), strict_slashes=False)
def post(post_id):
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    post = blog_backend.Post(post_id)
    if post.is_new_post:
        post = None
    
    user = blog_backend.get_logged_user(request.cookies)
    
    title = "Not found"
    if post != None:
        title = post.title
        if user:
            post.logged_user = user.username
    
    response = make_response(blog_backend.render_template(
        "index.html",
        content = blog_backend.render_template("post.html", post=post, user=user),
        title = title
    ))
    
    if user:
        user.update_session_id(response)
    return response

@app.route(urljoin(config.url["submit_comment_url"], '<post_id>'), methods=['GET', 'POST'], strict_slashes=False)
def submit_comment(post_id):
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    user = blog_backend.get_logged_user(request.cookies)
    if user == None or user.blocked == True:
        return "Forbidden"
    
    post = blog_backend.Post(post_id)
    
    post.add_comment(user, request.form["comment"])
    
    return redirect(urljoin(config.url["post_url"], post_id))

@app.route(urljoin(config.url["delete_comment_url"], '<post_id>', "<comment_id>"), strict_slashes=False)
def delete_comment(post_id, comment_id):
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    user = blog_backend.get_logged_user(request.cookies)
    if user == None or user.blocked == True:
        return "Forbidden"
    
    post = blog_backend.Post(post_id)
    post.delete_comment(user, int(comment_id))
    
    return redirect(urljoin(config.url["post_url"], post_id))

@app.route(urljoin(config.url["static_url"], "<post_id>", "<static_file>"), strict_slashes=False)
@app.route(urljoin(config.url["static_url"], "<static_file>"), strict_slashes=False)
def static_file(static_file, post_id=None):
    if post_id:
        return send_from_directory(os.path.join(config.posts_dir, post_id), static_file)
    else:
        return send_from_directory(config.static_dir, static_file)

@app.route(urljoin(config.url["static_page_url"], '<page_name>'), strict_slashes=False)
def static_page(page_name):
    file_path = os.path.join(config.static_pages_dir, page_name + ".yaml")
    
    if os.path.isfile(file_path):
        page = blog_backend.load_yaml_file(file_path)
        
        return blog_backend.render_template(
            "index.html",
            content = page["body"],
            title = page["title"]
        )
    
    else:
        return blog_backend.render_template(
            "index.html",
            content = "Page not found.",
            title = "Not found"
        )

@app.route(config.url["feed_url"], strict_slashes=False)
def feed():
    posts = blog_backend.sort(blog_backend.query(
        blog_backend.get_posts_list(),
        "list_post", "equal", True,
        "draft", "equal", False
    ), "date", True)
    
    posts = posts[0:config.atom_posts]
    
    update_time = datetime.utcnow().strftime(config.date_format)
    if len(posts) != 0:
        update_time = posts[0].date
    
    resp = make_response(blog_backend.render_template(
        "atom.xml",
        update_time=update_time,
        posts_list=posts
    ))
    
    resp.mimetype = "application/atom+xml"
    
    return resp

#user session
@app.route(urljoin(config.url["oauth2_login_url"], "<provider>"), strict_slashes=False)
def oauth2_login(provider):
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))

    client_secrets = config.oauth2_parameters[provider]
    
    state = hashlib.sha256(os.urandom(1024)).hexdigest()
    
    response = redirect(
        client_secrets["auth_uri"] + \
        "?client_id=" + client_secrets["client_id"] + \
        "&response_type=code&scope=" + client_secrets["scopes"] + \
        "&redirect_uri=" + urljoin(config.oauth2_redirect_uri, provider) + \
        "&state=" + state,
        code=302
    )
    
    response.set_cookie("state",
                        value=state,
                        expires=datetime.utcnow() + timedelta(seconds=120),
                        secure=config.secure_cookies,
                        httponly=True)
    
    response.set_cookie("source_url",
                        value=request.args.get("source_url", ""),
                        expires=datetime.utcnow() + timedelta(seconds=120),
                        secure=config.secure_cookies,
                        httponly=True)
    
    return response

@app.route('/oauth2callback/<provider>', strict_slashes=False)
def oauth2callback(provider):
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    client_secrets = config.oauth2_parameters[provider]
    
    if request.cookies.get("state") == request.args["state"]:
        r = requests.post(
            client_secrets["token_uri"],
            data = {
                "code": request.args["code"],
                "client_id": client_secrets["client_id"],
                "client_secret": client_secrets["client_secret"],
                "redirect_uri": urljoin(config.oauth2_redirect_uri, provider),
                "grant_type": "authorization_code",
                "state": request.cookies.get("state")
            },
            headers = {"Accept": "application/json"}
        )
        
        access_token = json.loads(r.text)["access_token"]
        
        r = requests.get(client_secrets["profile_api_url"] + \
            "?access_token=" + access_token)
        
        userinfo = json.loads(r.text)
        
        oauth2_id = "{}@{}".format(userinfo[client_secrets["id"]], provider)
        
        user = blog_backend.get_user_by_oauth2_id(oauth2_id)
        
        if user == None:
            user = blog_backend.User()
            
        if len(user.username) == 0:
            user.oauth2_id = oauth2_id
            user.name = userinfo[client_secrets["name"]]
            user.email = userinfo[client_secrets["email"]]
            user.picture = userinfo[client_secrets["picture_url"]]
            
            if provider == "github":
                r = requests.get("https://api.github.com/user/emails?access_token=" + access_token)
                emails = json.loads(r.text)
                try:
                    user.email = emails[0]["email"]
                except:
                    pass

        response = redirect(request.cookies.get("source_url"), code=302)
        user.update_session_id(response)
        return response
    else:
        return "Error"

@app.route(config.url["edit_profile_url"], methods=['GET', 'POST'], strict_slashes=False)
def edit_profile():
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    user = blog_backend.get_logged_user(request.cookies)
    
    if user == None:
        return redirect(config.url["index_url"], code=302)
    
    if request.method == 'POST':
        if request.form["username"] != user.username:
            return "Forbidden"
        
        redirect_url = config.url["index_url"]
        
        user.hide_picture = bool(request.form.get("hide_picture"))
        
        if len(request.form["name"]) > 0:
            user.name = request.form["name"]
        
        if user.level < 1:
            user.bio = request.form["bio"]
            redirect_url = config.url["admin_dashboard_url"]
        
        user.save()
        
        response = redirect(redirect_url, code=302)
        user.update_session_id(response)
        return response
    else:
        response = make_response(blog_backend.render_template(
            "index.html",
            content=blog_backend.render_template(
                "edit_profile.html",
                user=user,
                allow_bio_edit=user.level < 1,
            ),
            title="Edit Profile"
        ))
        
        user.update_session_id(response)
        return response

@app.route(config.url["logout_url"], strict_slashes=False)
def logout():
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    user = blog_backend.get_logged_user(request.cookies)
    
    if user != None:
        user.session_id = None
        user.save()
    
    if "redirect_url" in request.args:
        return redirect(request.args["redirect_url"], code=302)
    else:
        return redirect(config.url["index_url"], code=302)

#admin pages
def admin_required(func):
    def wrapped(*args, **kwds):
        if not request.is_secure and config.env != "debug":
            return redirect(request.url.replace("http://", "https://"))
        
        user = blog_backend.get_logged_user(request.cookies)
        
        if user == None:
            return redirect(config.url["admin_dashboard_url"], code=302)
        
        if user.level > 0:
            return "Forbidden"
        
        response = func(user, *args, **kwds)
        user.update_session_id(response)
        return response
    
    return wrapped

@app.route(config.url["admin_url"], strict_slashes=False)
def admin_redirect():
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    return redirect(urljoin(config.url["admin_dashboard_url"]), code=302)

@app.route(config.url["admin_dashboard_url"], strict_slashes=False)
def admin_dashboard():
    if not request.is_secure and config.env != "debug":
        return redirect(request.url.replace("http://", "https://"))
    
    user = blog_backend.get_logged_user(request.cookies)
    
    if user == None:
        return blog_backend.render_template(
            "index.html",
            content=blog_backend.render_template(
                "admin_dashboard.html",
                logged=False
            ),
            title="Login required"
        )
    
    if user.level > 0:
        return "Forbidden"
    
    response = make_response(blog_backend.render_template(
        "index.html",
        content=blog_backend.render_template(
            "admin_dashboard.html",
            logged=True,
            posts_list=blog_backend.sort(blog_backend.get_posts_list(), "date", True),
            users_list=blog_backend.sort(blog_backend.get_users_list(), "level", False),
        ),
        title="Dashboard"
    ))
    user.update_session_id(response)
    return response

@app.route(config.url["edit_post_url"], endpoint="post_actions",  methods=['POST'], strict_slashes=False)
@app.route(urljoin(config.url["edit_post_url"], "<post_id>"), endpoint="post_actions",  methods=['GET', 'POST'], strict_slashes=False)
@app.route(config.url["new_post_url"], endpoint="post_actions",  methods=['GET', 'POST'], strict_slashes=False)
@admin_required
def admin_post_actions(user, post_id=None):
    if request.method == 'POST':
        post = blog_backend.Post(post_id)
        post.author = user.username
        post.title = request.form["title"]
        post.edit_body = request.form["body"]
        post.tags = [re.sub("\s\s+", " ", tag.strip()) for tag in request.form["tags"].split(";") if len(tag.strip()) > 0]
        post.draft = bool(request.form.get("draft"))
        post.list_post = bool(request.form.get("list_post"))
        post.allow_comments = bool(request.form.get("allow_comments"))
        
        if "upload_file" in request.form:
            
            #save post as draft, save uploaded file and redirect to edit page
            post.draft = True
            post.save()
            
            if 'file' in request.files:
                file = request.files['file']
                if file.filename != '' and file:
                    post.save_static_file(file)
            
            return redirect(urljoin(config.url["edit_post_url"], post.id), code=302)
        
        post.save()
        
        return redirect(config.url["admin_dashboard_url"], code=302)
    else:
        post = blog_backend.Post(post_id)
        title = "Edit post"
        
        if not post_id:
            title = "New post"
        
        return make_response(blog_backend.render_template(
            "index.html",
            content=blog_backend.render_template(
                "admin_edit_post.html",
                post=post
            ),
            title=title
        ))

@app.route(urljoin(config.url["delete_post_url"], "<post_id>"), endpoint="delete_post", strict_slashes=False)
@admin_required
def admin_delete_post(user, post_id):
    blog_backend.Post(post_id).delete()
    
    return redirect(config.url["admin_dashboard_url"], code=302)

@app.route(urljoin(config.url["delete_post_static_url"], "<post_id>", "<filename>"), endpoint="delete_post_static", strict_slashes=False)
@admin_required
def admin_delete_static_file(user, post_id, filename):
    blog_backend.Post(post_id).delete_static_file(filename)
    
    return redirect(urljoin(config.url["edit_post_url"], post_id), code=302)

@app.route(urljoin(config.url["toggle_block_user_url"], "<user>"), endpoint="block_user", strict_slashes=False)
@admin_required
def toggle_block_user(admin, user):
    if user != admin.username:
        user = blog_backend.User(user)
        user.blocked = not user.blocked
        user.save()
    
    return redirect(config.url["admin_dashboard_url"], code=302)
    
@app.route(urljoin(config.url["toggle_hide_content_url"], "<user>"), endpoint="hide_content", strict_slashes=False)
@admin_required
def toggle_hide_content(admin, user):
    if user == admin.username:
        user = admin
    else:
        user = blog_backend.User(user)
    
    user.hide_content = not user.hide_content
    user.save()
    
    return redirect(config.url["admin_dashboard_url"], code=302)
    
@app.route(config.url["backup_url"], endpoint="backup", strict_slashes=False)
@admin_required
def admin_backup(user):
    backup = blog_backend.backup()
    
    return send_file(backup, attachment_filename='backup.zip', as_attachment=True)

@app.route(config.url["restore_backup_url"], endpoint="restore_backup",  methods=['POST'], strict_slashes=False)
@admin_required
def admin_restore_backup(user):
    print(request.form, request.files)
    if "restore_backup" in request.form:
        if 'file' in request.files:
            file = request.files['file']
            if file.filename != '' and file:
                if blog_backend.restore_backup(file):
                    return make_response("Backup restored")
                else:
                    return make_response("The backup file is invalid.")
    
    return make_response("The backup file is invalid.")

if __name__ == '__main__':
    app.run()
