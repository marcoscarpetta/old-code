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
import sys
import shutil
import ruamel.yaml as yaml

application_files = [
    "blog_app.py",
    "blog_backend.py",
    "config.py",
    "setup.py",
    "wsgi.py",
    "requirements.txt",
    "templates/admin_dashboard.html",
    "templates/admin_edit_post.html",
    "templates/edit_profile.html",
]

editable_files = [
    "templates/atom.xml",
    "templates/index.html",
    "templates/page.html",
    "templates/post.html",
    "config.yaml"
]

mkdirs = [
    "templates"
]

def check_files():
    for file in application_files:
        if not os.path.isfile(file):
            print('ERROR: File "{}" not found.'.format(file))
            return False
    
    for file in editable_files:
        if not os.path.isfile(file):
            print('ERROR: File "{}" not found.'.format(file))
            return False
    
    return True

def install(dest_dir):
    for dir in mkdirs:
        if not os.path.isdir(os.path.join(dest_dir, dir)):
            os.mkdir(os.path.join(dest_dir, dir))
    
    for file in application_files:
        shutil.copy(file, os.path.join(dest_dir, file))
    
    for file in editable_files:
        if not os.path.exists(os.path.join(dest_dir, file)):
            shutil.copy(file, os.path.join(dest_dir, file))

def check_dict(source, dest, key_path):
    error = False
    
    for key in source:
        if key not in dest:
            print("ERROR: Your config file is missing the '{} -> {}' key".format(key_path, key))
            error = True
        elif type(source[key]) != type(dest[key]):
            print("ERROR: Wrong type for the '{} -> {}' key in your config file".format(key_path, key))
            error = True
        elif type(source[key]) == dict:
            error = check_dict(source[key], dest[key], key_path + " -> " + key) or error
    
    return error

def check_config(dest_dir):
    f = open(os.path.join(dest_dir, "config.yaml"), "r", encoding='utf-8')
    dest = yaml.safe_load(f.read())
    f.close()
    
    f = open("config.yaml", "r", encoding='utf-8')
    source = yaml.safe_load(f.read())
    f.close()
    
    return not check_dict(source, dest, "")

if __name__ == '__main__':
    if len(sys.argv) == 2:
        if check_files():
            install(sys.argv[1])
            
            if check_config(sys.argv[1]):
                print("flat-flask-blog successfully installed!")
            else:
                print()
                print("flat-flask-blog was successfully installed, but you NEED to fix some errors in your config file")
        else:
            print()
            print("There were some errors during installation")
    else:
        print("You must pass only the install destination directory path")
