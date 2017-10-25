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
from posixpath import join as urljoin
import ruamel.yaml as yaml

config_file_path = "config.yaml"

if "OPENSHIFT_REPO_DIR" in os.environ:
    config_file_path = os.path.join(os.environ["OPENSHIFT_REPO_DIR"], "config.yaml")

f = open(config_file_path, "r", encoding='utf-8')
data = yaml.safe_load(f.read())
f.close()

#display options
posts_per_page = data["posts_per_page"]
atom_posts = data["atom_posts"]
date_format = data["date_format"]
display_date_format = data["display_date_format"]

#environment variables
root_dir = ""
persistent_data_dir = ""
root_url = ""
secure_root_url = ""
secure_cookies = True
env = "deployment"

if "DEBUG" in os.environ and os.environ["DEBUG"] == "1":
    secure_cookies = False
    env = "debug"

if "env:" in data["environment"][env]["root_dir"]:
    root_dir = os.environ[data["environment"][env]["root_dir"].replace("env:", "").strip()]
else:
    root_dir = data["environment"][env]["root_dir"]

if "env:" in data["environment"][env]["persistent_data_dir"]:
    persistent_data_dir = os.environ[data["environment"][env]["persistent_data_dir"].replace("env:", "").strip()]
else:
    persistent_data_dir = data["environment"][env]["persistent_data_dir"]

root_url = data["environment"][env]["root_url"]
secure_root_url = data["environment"][env]["secure_root_url"]

#paths
posts_dir = os.path.join(persistent_data_dir, data["posts_dir"])
users_dir = os.path.join(persistent_data_dir, data["users_dir"])
usernames_file = os.path.join(persistent_data_dir, data["usernames_file"])

if not os.path.isdir(posts_dir):
    os.mkdir(posts_dir)

if not os.path.isdir(users_dir):
    os.mkdir(users_dir)

if not os.path.isfile(usernames_file):
    open(usernames_file, "w").close()

static_dir = os.path.join(root_dir, data["static_dir"])
templates_dir = os.path.join(root_dir, data["templates_dir"])
static_pages_dir = os.path.join(root_dir, data["static_pages_dir"])

#URLs
url = {}
url["index_url"] = data["urls"]["index_url"]

for url_name in ["page_url", "post_url", "submit_comment_url", "delete_comment_url",
    "tag_url", "author_url", "static_url", "static_page_url", "feed_url",
    "oauth2_login_url", "logout_url", "edit_profile_url", "oauth2_redirect_url",
    "admin_url"]:
    url[url_name] = urljoin(url["index_url"], data["urls"][url_name])

for url_name in ["admin_dashboard_url", "new_post_url", "edit_post_url",
    "delete_post_url", "delete_post_static_url", "toggle_block_user_url",
    "toggle_hide_content_url", "backup_url", "restore_backup_url"]:
    url[url_name] = urljoin(url["admin_url"], data["urls"][url_name])

oauth2_redirect_uri = secure_root_url.rstrip("/") + "/" + url["oauth2_redirect_url"].lstrip("/")

#root user id on the OAuth 2.0 provider
root_user_id = data["root_user_id"]
root_user_provider = data["root_user_provider"]

oauth2_parameters = data["oauth2_parameters"]
