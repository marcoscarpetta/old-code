import datetime
import os

date_format = "%Y/%m/%d %H:%M"

properties = {
    "list_post": True,
    "title": "Title",
    "author": "Author",
    "date": None,
    "edit_date": None,
    "body": "Body",
    "tags": [],
}

class Post:
    def __init__(self, posts_dir, uid):
        
        self._id = uid
        
        for key in properties:
            setattr(self, key, properties[key])
        
        self._path_to_file = os.path.join(posts_dir, uid, "index.html")
        self.parse()
    
    #id
    @property
    def id(self):
        return self._id
    
    @id.setter
    def id(self, value):
        print("WARNING: a post's id cannot be changed")
    
    #list_post
    @property
    def list_post(self):
        return bool(self._list_post)
    
    @list_post.setter
    def list_post(self, value):
        try:
            self._list_post = int(value)
        except:
            self._list_post = bool(value)
    
    #title
    @property
    def title(self):
        return self._title
    
    @title.setter
    def title(self, value):
        self._title = str(value)
    
    #author
    @property
    def author(self):
        return self._author
    
    @author.setter
    def author(self, value):
        self._author = str(value)
    
    #date
    @property
    def date(self):
        return self._date
    
    @date.setter
    def date(self, value):
        if isinstance(value, str):
            try:
                date_string = value.split("\n")[0].lstrip().rstrip()
                self._date = datetime.datetime.strptime(date_string, date_format)
            except:
                print('\nERROR: Invalid date string "'+value+'" in file '+self._path_to_file, "\n")
                raise
                
        elif isinstance(value, datetime.datetime):
            self._date = value
        else:
            self._date = None
    
    #edit-date
    @property
    def edit_date(self):
        return self._edit_date
    
    @edit_date.setter
    def edit_date(self, value):
        if isinstance(value, str):
            try:
                date_string = value.split("\n")[0].lstrip().rstrip()
                self._edit_date = datetime.datetime.strptime(date_string, date_format)
            except ValueError:
                print('\nERROR: Invalid edit-date string "', value, '" in file', self._path_to_file, "\n")
                raise
                
        elif isinstance(value, datetime.datetime):
            self._edit_date = value
        else:
            self._edit_date = None
    
    #body
    @property
    def body(self):
        return self._body
    
    @body.setter
    def body(self, value):
        self._body = str(value)
    
    #tags
    @property
    def tags(self):
        return self._tags
        
    @tags.setter
    def tags(self, value):
        if isinstance(value, list):
            self._tags = value
                
        elif isinstance(value, str):
            self._tags = []
            split = value.split(",")
            for tag in split:
                tag = tag.lstrip().rstrip()
                
                if tag != "":
                    self.tags.append(tag)
                else:
                    self._tags = []
    
    def parse(self):
        try:
            f = open(self._path_to_file, "r")
        except IOError:
            print('\nERROR: Cannot open file', self._path_to_file, "\n")
            raise
            
        line = f.readline();
        
        currentValue = ""
        currentKey = None
        
        while line != "":
            stripped_line = line.replace("\n", "").lstrip().rstrip()
            if stripped_line in properties:
                if currentKey :
                    value = currentValue.rstrip("\n").lstrip("\n")
                    if value == "True":
                        value = True
                    elif value == "False":
                        value = False
                    elif value == "None":
                        value = None
                        
                    setattr(self, currentKey, value)
            
                currentKey = stripped_line
                currentValue = ""
            
            else:
                currentValue += line
            
            line = f.readline()

        if currentKey :
            setattr(self, currentKey, currentValue)
            
        f.close()

        #set post date to last edit date of the file
        if not self.date:
            self.date = datetime.datetime.fromtimestamp(os.path.getmtime(self._path_to_file))
        
        self.save()
                                                                                               
    def save(self):
        tmp = "list_post\n" + str(self.list_post) + "\n\n"
        tmp += "title\n" + self.title + "\n\n"
        tmp += "author\n" + self.author + "\n\n"
        tmp += "date\n" + self.date.strftime(date_format) + "\n\n"
        if self.edit_date:
            tmp += "edit_date\n" + self.edit_date.strftime(date_format) + "\n\n"
        else:
            tmp += "edit_date\nNone\n\n"
        if len(self.tags) > 0:
            tmp += "tags\n"
            for tag in self.tags:
                tmp += tag + ", "
            tmp = tmp.rstrip(", ") + "\n\n"
        tmp += "body\n" + self.body
        
        f = open(self._path_to_file, "w")
        f.write(tmp)
        f.close()

class PostCollection():

    def __init__(self, posts_dir):
        self._posts_dir = posts_dir
        
        self._tags = []
        self._authors = []
        
        self.load_posts()
        
        self.clear_query()
    
    def load_posts(self):
        self._posts = []
        
        for entry in os.scandir(self._posts_dir):
            if not entry.name.startswith('.') and entry.is_dir():
                if os.path.exists(os.path.join(entry.path, "index.html")):
                    post = Post(self._posts_dir, entry.name)
                    
                    for tag in post.tags:
                        if tag not in self._tags:
                            self._tags.append(tag)
                    if post.author not in self.authors:
                        self._authors.append(post.author)
                    
                    self._posts.append(post)
    
    def sort(self, key, descending=True):
        self._query_sort = [key, descending]
        return self
    
    def filter(self, **args):
        filters = {
            "equal": self._query_equal,
            "lt": self._query_lt,
            "gt": self._query_gt,
            "contains": self._query_contains
        }
        
        for key in args:
            split = key.split("__")
            if len(split) != 2:
                raise Exception("ERROR: invalid filter " + key + "=" + args[key])
            if split[0] not in properties or split[1] not in filters:
                raise Exception("ERROR: invalid filter " + key + "=" + args[key])
            
            filters[split[1]][split[0]] = args[key]
        
        return self
    
    def clear_query(self):
        self._query_equal = {}
        self._query_gt = {}
        self._query_lt = {}
        self._query_contains = {}
        self._query_sort = None
    
    #execute query when accessing the query attribute
    @property
    def query(self):
        posts = list(self._posts)
        
        #equal queries
        for key in self._query_equal:
            i = len(posts)-1
            while i >= 0:
                if getattr(posts[i], key) != self._query_equal[key]:
                    posts.pop(i)
                i -= 1
        
        #lt queries
        for key in self._query_lt:
            i = len(posts)-1
            while i >= 0:
                if getattr(posts[i], key) > self._query_lt[key]:
                    posts.pop(i)
                i -= 1
        
        #gt queries
        for key in self._query_gt:
            i = len(posts)-1
            while i >= 0:
                if getattr(posts[i], key) < self._query_gt[key]:
                    posts.pop(i)
                i -= 1
        
        #contains queries
        for key in self._query_contains:
            i = len(posts)-1
            while i >= 0:
                attr = getattr(posts[i], key)
                if isinstance(attr, str) or isinstance(attr, list):
                    if self._query_contains[key] not in attr:
                        posts.pop(i)
                else:
                    print("WARNING: query", key, "__contains not executed")
                i -= 1
        
        #sort
        if self._query_sort:
            key = self._query_sort[0]
            i = 0
            while i < len(posts):
                min_max = i
                j = i
                while j < len(posts):
                    if (getattr(posts[j], key) > getattr(posts[min_max], key)) == self._query_sort[1]:
                        min_max = j;
                    j += 1

                if min_max != i:
                    tmp = posts[i]
                    posts[i] = posts[min_max]
                    posts[min_max] = tmp
                i += 1
        
        self.clear_query()
        return posts
    
    @query.setter
    def query(self, value):
        print("WARNING: the query attribute is read-only")
        
    #authors
    @property
    def authors(self):
        return self._authors
    
    @authors.setter
    def authors(self, value):
        print("WARNING: the authors attribute is read-only")
        
    #tags
    @property
    def tags(self):
        return self._tags
    
    @tags.setter
    def tags(self, value):
        print("WARNING: the tags attribute is read-only")
        