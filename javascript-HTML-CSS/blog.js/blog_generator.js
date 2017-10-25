window.addEventListener('load', function() {
  var pars = parseUrl();
  //Edit the code belowe
  //specify the path to the json file
  var blog = new Blog("db.json");
  //sort posts by a field (e.g. date)
  blog.sort('date', false);
  //modify some control variable
  blog.posts_per_page = 3;
  blog.max_preview_length = 2000;
  //change the page title
  document.title = blog.getTitle(pars) + " | " + document.title;
  //load the blog
  var blog_div = document.getElementById("blog.js");
  blog_div.parentNode.replaceChild(blog.getView(pars), blog_div);
}, false);