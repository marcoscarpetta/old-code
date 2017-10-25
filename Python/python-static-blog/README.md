# python-static-blog
A static blog generator written in Python 3

##Usage

$ python export.py --help

usage: export.py [-h] [--output_dir OUTPUT_DIR] [--posts_dir POSTS_DIR] [--templates_dir TEMPLATES_DIR]

A static blog generator

optional arguments:

**-h, --help**

show this help message and exit

**--output_dir OUTPUT_DIR**

output directory

**--posts_dir POSTS_DIR**

posts source directory

**--templates_dir TEMPLATES_DIR**

templates directory

## Post syntax

Example file:
```
list_post
True|False|0|1

title
<b>html allowed</b>

author
Author

date
YYYY/MM/DD HH:MM

edit_date
None|YYYY/MM/DD HH:MM

tags
tag 1, tag 2, tag 3

body
<p>
html allowed
</p>
```

Every field is optional. If no date is provided it will be set to the last edit date of the file.

Files must be organized in a directory like this:
```
posts
|
\- post 1 id
|  |
|  \- index.html
|
\- post 2 id
   |
   \- index.html
  
