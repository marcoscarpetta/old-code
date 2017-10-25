/*  
 *	@licstart  The following is the entire license notice for the 
 *    JavaScript code in this page.
 *    
 *    Copyright (C) 2013  Marco Scarpetta
 * 
 *    The JavaScript code in this page is free software: you can
 *    redistribute it and/or modify it under the terms of the GNU
 *    General Public License (GNU GPL) as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.  The code is distributed WITHOUT ANY WARRANTY;
 *    without even the implied warranty of MERCHANTABILITY or FITNESS
 *    FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.
 * 
 *    As additional permission under GNU GPL version 3 section 7, you
 *    may distribute non-source (e.g., minimized or compacted) forms of
 *    that code without the copy of the GNU GPL normally required by
 *    section 4, provided you include this license notice and a URL
 *    through which recipients can access the Corresponding Source.
 *	
 *    @licend  The above is the entire license notice
 *    for the JavaScript code in this page.
 */


/**
 * Parses the current URL.
 * @return {Object} An Object representing the Url
 */
function parseUrl() {
  if (document.URL.contains('?')) {
    obj = new Object();
    var pars = document.URL.split('?')[1];
    var vars = pars.split('&');
    for (var i = 0; i < vars.length; i++) {
      var pair = vars[i].split('=');
      obj[decodeURIComponent(pair[0])] = 
      decodeURIComponent(pair[1]);
    }
    return obj;
  }
  return null;
}

/**
 * Returns the Url representing the given Object.
 * <p>
 * Example: <br>
 * {'view': 'tag', 'tag': 'test'} --> '?view=tag&tag=test'
 * </p>
 * @param {Object} pars An object representing the Url
 * @return {String} A string representing the given Url
 */
function getUrl(pars) {
  url = '?';
  for (var key in pars) {
    url += encodeURIComponent(key)+'=';
    url += encodeURIComponent(pars[key])+'&';
  }
  return url.slice(0,url.length-1);
}

/**
 * Loads a file synchronously
 * @param {String} path_to_file The path of the file
 * @return {String} The loaded file
 */
function loadFile(path_to_file) {  
  if (window.XMLHttpRequest){xmlhttp=new XMLHttpRequest();}
  else {xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");}
  
  xmlhttp.overrideMimeType('text/plain');
  xmlhttp.open("GET", path_to_file, false);
  xmlhttp.send();
  return xmlhttp.responseText;
}

/**
 * Creates an instance of Post
 * @constructor
 * @param {Object} post_object The post object (same format of the json
 * file)
 */
function Post(post_object) {
  this.id = post_object.id;
  this.date = new Date(post_object.date);
  this.title = post_object.title;
  this.author = post_object.author;
  this.tags = post_object.tags;
  this.body = loadFile(post_object.body_path);
}

/**
 * Returns the post's date. Can be overridden
 * @return {String} The post's date
 */
Post.prototype.getDate = function() {
  return this.date.toLocaleString()
}

/**
 * Returns the post's body
 * @param {Number} max_length The maximum post's length. If negative,
 * returns the full body
 * @return {String} The post's body
 */
Post.prototype.getBody = function(max_length) {
  if (max_length>0 && this.body.length > max_length) {
    //FIXME Should check tags
    return this.body.slice(0, max_length)+'...';
  }
  return this.body;
}

/**
 * Creates an instance of Blog
 * @constructor
 * @param {String} path_to_json The path to the json file
 */
function Blog (path_to_json) {
  //maximum preview length
  this.max_preview_length = 20;
  //posts per page in list views
  this.posts_per_page = 5;
  this.db = JSON.parse(loadFile(path_to_json)).posts;
}

/**
 * Sorts the Blog database
 * @param {String} field The field by which order
 * @param {Boolean} ascending Orders ascending if true
 */
Blog.prototype.sort = function(field, ascending) {
  for (var i=0; i<this.db.length-1; i++) {
    var min_max = i;
    for (var j=i; j<this.db.length; j++) {
      if ((this.db[j][field] < this.db[min_max][field])
	== ascending) {
	min_max = j;
	}
    }
    if (min_max != i) {
      var tmp = this.db[i];
      this.db[i] = this.db[min_max];
      this.db[min_max] = tmp;
    }
  }
}

/**
 * Returns a list of objects corresponding to the query
 * <p>
 * Supported filters:<br>
 * <ul>
 * <li>equal (Any type)</li>
 * <li>contains (String and Array)</li>
 * </ul><br>
 * Examples:<br>
 * blog.query({'field': 'id', 'equal': 0});<br>
 * blog.query({'field': 'tags', 'contains': 'test'});
 * </p>
 * @param {Object} query An Object representing the query
 * @return {Array} An Array of Objects correspondig to the query. Can be
 * void.
 */
Blog.prototype.query = function(query) {
  var list = [];
  if (query.equal) {
    for (var i in this.db) {
      if (this.db[i][query.field] == query.equal) {
	list.push(this.db[i]);
      }
    }
  }
  else if (query.contains) {
    for (var i in this.db) {
      if (this.db[i][query.field].contains) {
	if (this.db[i][query.field].contains(query.contains)) {
	  list.push(this.db[i]);
	}
      }
      else if (this.db[i][query.field].indexOf) {
	if (this.db[i][query.field].indexOf(query.contains)>=0)
	{
	  list.push(this.db[i]);
	}
      }
    }
  }
  return list;
}

/**
 * Returns a Post object of the post with the given id
 * @param {Number} id The post's id
 * @return {Post} The post with the given id or null if it doesn't 
 * exists
 */
Blog.prototype.getPost = function(id) {
  for (var i=0; i<this.db.length; i++) {
    if (id == this.db[i].id) {
      return new Post(this.db[i]);
    }
  }
  return null;
}

/**
 * Returns a representation of the post
 * @param {Post} post The post to render
 * @param {Boolean} preview Render the post as preview if true
 * @return {HTMLElement} The post
 */
Blog.prototype.getPostView = function(post, preview) {
  var article = document.createElement('article'); //article tag
  article.className = 'post';
  var header = document.createElement('header'); //post's haeder tag
  var title = document.createElement('h1'); //post title
  title.className = 'post_title';
  title.title = post.title;
  title.appendChild(document.createTextNode(post.title));
  var aut_date = document.createElement('p'); //author and date
  aut_date.className = 'author_date';
  aut_date.appendChild(document.createTextNode('Written by '));
  var author = document.createElement('a'); //author
  author.className = 'author';
  author.href = getUrl({'view': 'author',
    'author': post.author, 'page': 0});
  author.appendChild(document.createTextNode(post.author));
  aut_date.appendChild(author);
  aut_date.appendChild(document.createTextNode(' on '));
  var date = document.createElement('span'); //date
  date.className = 'date';
  date.appendChild(document.createTextNode(post.getDate()));
  aut_date.appendChild(date);
  var tags = document.createElement('div'); //tags
  tags.className = 'tags';
  for (var i in post.tags) {
    var a = document.createElement('a');
    a.appendChild(document.createTextNode(post.tags[i]));
    a.href = getUrl({'view': 'tag', 'tag': post.tags[i],
      'page': 0});
    a.title = 'Tag: '+post.tags[i];
    a.className = 'tag';
    tags.appendChild(a);
    tags.appendChild(document.createTextNode(' '));
  }
  
  
  //If `preview` is `true`, the title is a link to the post view
  if (preview) {
    a = document.createElement('a');
    a.class_name = 'post_title_link';
    a.href = getUrl({'view': 'post', 'id': post.id});
    a.appendChild(title);
    header.appendChild(a);
  }
  else {
    header.appendChild(title);
  }
  
  article.appendChild(header);
  article.appendChild(aut_date);
  article.appendChild(document.createElement('br'));
  article.appendChild(tags);
  
  article.innerHTML += post.getBody(preview ? 
  this.max_preview_length : -1);
  
  return article;
}

/**
 * Returns a representation of the list of posts
 * @param {Array} list The list of posts to render
 * @param {Object} pars The url's parameters
 * @return {HTMLElement} The list of posts
 */
Blog.prototype.getListView = function(list, pars) {
  var first = pars.page*this.posts_per_page;
  var posts = list.slice(first, first+this.posts_per_page+1);
  var div = document.createElement('div');
  var last = (posts.length>this.posts_per_page) ?
  posts.length-1 : posts.length;
  for (var i=0; i<last; i++) {
    var post = new Post(posts[i]);
    div.appendChild(this.getPostView(post, true));
  }
  
  if (pars.page>0) {
    var a = document.createElement('a');
    a.className = 'next_posts';
    pars.page -=  1;
    a.href=getUrl(pars);
    a.appendChild(document.createTextNode('Next posts'));
    pars.page += 1;
    div.appendChild(a);
  }
  if (posts.length>this.posts_per_page) {
    var a = document.createElement('a');
    a.className = 'prev_posts';
    pars.page +=  1;
    a.href=getUrl(pars);
    a.appendChild(document.createTextNode('Previous posts'));
    div.appendChild(a);
  }
  return div;
}

/**
 * Returns a representation of the view
 * @param {Object} pars The url's parameters
 * @return {HTMLElement} The view
 */
Blog.prototype.getView = function(pars) {
  if (pars == null) {
    var posts = this.query({'field': 'list',
      'equal': true});
    pars = {'view': 'home', 'page': 0}
    return this.getListView(posts, pars)
  }
  if (pars.view == 'home') {
    var posts = this.query({'field': 'list',
      'equal': true});
    return this.getListView(posts, pars);
  }
  if (pars.view == 'author') {
    var posts = this.query({'field': 'author',
      'equal': pars.author});
    var div = this.getListView(posts, pars);
    text = document.createTextNode('Posts from: '+pars.author);
    div.insertBefore(text, div.firstChild);
    return div;
  }
  if (pars.view == 'tag') {
    var posts = this.query({'field': 'tags',
      'contains': pars.tag});
    var div = this.getListView(posts, pars);
    text = document.createTextNode('Tag: '+pars.tag);
    div.insertBefore(text, div.firstChild);
    return div;
  }
  if (pars.view == "post") {
    var post = this.getPost(parseInt(pars.id));
    return this.getPostView(post, false);
  }
}

/**
 * Returns the title of the page
 * @param {Object} pars The url's parameters
 * @return {String} The page's title
 */
Blog.prototype.getTitle = function(pars) {
  if (pars == null) {
    return "Home";
  }
  if (pars.view == 'home') {
    return "Home";
  }
  if (pars.view == 'author') {
    return "Author: "+pars.author;
  }
  if (pars.view == 'tag') {
    return "Tag: "+pars.tag;
  }
  if (pars.view == "post") {
    var post = this.getPost(parseInt(pars.id));
    return post.title;
  }
}
