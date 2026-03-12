#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#define INDEX_HTML \
"<!DOCTYPE html>"\
"<html>"\
"<head>"\
    "<title>webtest</title>"\
"</head>"\
"<body>"\
    "<h1>Information about program</h1>"\
\
    "<p>HTTP test server for measuring download speed.</p>"\
    "<p>Generate files of arbitrary size by specifying the desired size in the URL:</p>"\
    "<pre>http://host:port/&lt;size&gt;&lt;B|K|M|G&gt;</pre>"\
\
    "<h2>Example</h2>"\
    "<pre>time curl http://192.168.1.2:8080/10M &gt; /dev/null</pre>"\
\
    "<h2>Size limits</h2>"\
    "<p>Min: 0B <br> Max: 50G</p>"\
\
    "<h2>Try it right now</h2>"\
    "<strong>Warning: Don't open after download (large files may freeze)</strong>"\
    "<br>"\
    "<a href=\"/100B\">/100B</a>"\
    "<br>"\
    "<a href=\"/100K\">/100K</a>"\
    "<br>"\
    "<a href=\"/100M\">/100M</a>"\
    "<br>"\
    "<a href=\"/1G\">/1G</a>"\
    "<h2>Repository</h2>"\
    "<p>More info: <a href=\"https://github.com/zverorx/webtest\">github.com/zverorx/webtest</a></p>"\
"</body>"\
"</html>"

#endif /* INDEX_HTML_H */
