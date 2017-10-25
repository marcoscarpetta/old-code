import db
import os, random, string, shutil
import datetime, math
import argparse
import re

class Exporter():
    def __init__(self, output_dir, templates_dir, posts_dir):
        self.output_dir = output_dir
        self.posts_dir = posts_dir
        self.templates_dir = templates_dir
        
        #settings
        self.posts_per_page = 4
        self.preview = "first_p"
        self.date_format = "%Y/%m/%d %H:%M"
        
        #load tamplates
        self.frame_template = open(os.path.join(templates_dir, "frame.html")).read()
        self.post_template = open(os.path.join(templates_dir, "post.html")).read()
        self.post_preview_template = open(os.path.join(templates_dir, "post_preview.html")).read()
        self.home_template = open(os.path.join(templates_dir, "home.html")).read()
        self.tag_template = open(os.path.join(templates_dir, "tag.html")).read()
        self.author_template = open(os.path.join(templates_dir, "author.html")).read()

        self.posts = db.PostCollection(posts_dir)

    def get_post_dict(self, post, base_path):
        regex = re.compile('href="(?!https{0,1}://)')
        body = regex.sub('href="{}{}/'.format(base_path, post.id), post.body)
        regex = re.compile('src="(?!https{0,1}://)')
        body = regex.sub('src="{}{}/'.format(base_path, post.id), body)
        
        post_dict = {
            "title": post.title,
            "author": '<a href="{path}author/{author}/page1.html">{author}</a>'\
                .format(path=base_path, author=post.author),
            "date": post.date.strftime(self.date_format),
            "body": body,
            "tags": "",
            "link": base_path+post.id+"/index.html"
        }
                
        for tag in post.tags:
            post_dict["tags"] += '<a href="{path}tag/{tag}/page1.html">{tag}</a> '\
                .format(path=base_path, tag=tag)
        
        return post_dict

    def get_pages(self, posts, up=0):
        base_path = "../"*up
        
        pages = []

        n_pages = math.ceil(len(posts)/self.posts_per_page)
        
        for p in range(1, n_pages+1):
            post_previews = ""
            for i in range((p-1)*self.posts_per_page, min(p*self.posts_per_page, len(posts))):
                post_dict = self.get_post_dict(posts[i], base_path)
                
                post_previews += self.post_preview_template.format(**post_dict)
                
            page_indicator = ""
            if p != 1:
                page_indicator += '<a href="page{page}.html">Previous page</a>'.format(page=p-1)
            page_indicator += "Page {page} of {total}".format(page=p, total=n_pages)
            if p != n_pages:
                page_indicator += '<a href="page{page}.html">Next page</a>'.format(page=p+1)
            
            pages.append({"page_number":p, "posts":post_previews, "pages":page_indicator})
        
        return pages

    def export(self):
        
        backup_dir = ""

        #backup output directory, if any
        if os.path.isdir(self.output_dir):
            backup_dir = "backup_"+datetime.datetime.now().strftime("%Y%m%d-%H%M%s")
            os.rename(self.output_dir, backup_dir)
                
        os.mkdir(self.output_dir)

        #generate single post views
        for post in self.posts.query:
            os.mkdir(os.path.join(self.output_dir, post.id))
            
            #copy post's files to output directory
            for entry in os.scandir(os.path.join(self.posts_dir, post.id)):
                if not entry.name.startswith('.') and not entry.is_dir():
                    shutil.copy(entry.path, os.path.join(self.output_dir, post.id))
            
            post_dict = self.get_post_dict(post, "../")
            
            page = self.post_template.format(**post_dict)
            page = self.frame_template.format(content=page, home_path="../")
            
            f = open(os.path.join(self.output_dir, post.id, "index.html"), "w")
            f.write(page)
            f.close()

        #generate home page
        self.posts.filter(list_post__equal=True).sort("date")
        for page in self.get_pages(self.posts.query):
            html = self.home_template.format(posts=page["posts"], pages=page["pages"])
            html = self.frame_template.format(content=html, home_path="")
            f = open(os.path.join(self.output_dir, "page%d.html" % page["page_number"]), "w")
            f.write(html)
            f.close()
        shutil.copyfile(os.path.join(self.output_dir, "page1.html"),
                        os.path.join(self.output_dir, "index.html"))

        #generate tag pages
        os.mkdir(os.path.join(self.output_dir, "tag"))
        for tag in self.posts.tags:
            os.mkdir(os.path.join(self.output_dir, "tag", tag))
            self.posts.filter(tags__contains=tag).sort("date")
            for page in self.get_pages(self.posts.query, 2):
                html = self.tag_template.format(tag=tag, posts=page["posts"], pages=page["pages"])
                html = self.frame_template.format(content=html, home_path="../../")
                f = open(os.path.join(self.output_dir, "tag", tag,
                                      "page{}.html".format(page["page_number"])), "w")
                f.write(html)
                f.close()
        
        #generate author pages
        os.mkdir(os.path.join(self.output_dir, "author"))
        for author in self.posts.authors:
            os.mkdir(os.path.join(self.output_dir, "author", author))
            self.posts.filter(author__equal=author).sort("date")
            for page in self.get_pages(self.posts.query, 2):
                html = self.author_template.format(author=author, posts=page["posts"], pages=page["pages"])
                html = self.frame_template.format(content=html, home_path="../../")
                f = open(os.path.join(self.output_dir, "author", author,
                                      "page{}.html".format(page["page_number"])), "w")
                f.write(html)
                f.close()
        
        if backup_dir != "":
            shutil.rmtree(backup_dir)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="A static blog generator")
    
    parser.add_argument("--output_dir", type=str, help="output directory", default="out")
    parser.add_argument("--posts_dir", type=str, help="posts source directory", default="posts")
    parser.add_argument("--templates_dir", type=str, help="templates directory", default="templates")
    
    args = parser.parse_args()

    output_dir = os.path.join(os.getcwd(), args.output_dir)
    templates_dir = os.path.join(os.getcwd(), args.templates_dir)
    posts_dir = os.path.join(os.getcwd(), args.posts_dir)
    
    for dir in (templates_dir, posts_dir):
        if not os.path.isdir(dir):
            raise Exception("ERROR: directory {} does not exists".format(dir))
    
    exporter = Exporter(output_dir, templates_dir, posts_dir)
    exporter.export()