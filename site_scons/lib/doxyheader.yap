<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>$title</title>
<link href="@TOPDIR@/doc/html/doxygen.css" rel="stylesheet" type="text/css">
<link href="@TOPDIR@/doc/doxy.css" rel="stylesheet" type="text/css">
</head>
<body>
<div id="head">
  <div id="title">
    <div id="title2">
      <h1>${PROJECTNAME}</h1>
    </div>
  </div>
  <div id="subtitle">
    <ul>
      <li><a href="index.html">Home</a></li>
{{    for name, target in DOCLINKS:
        <li><a href="${target}">${name}</a></li>
}}
    </ul>
  </div>
</div>

<div id="content1">
  <div id="content2">
