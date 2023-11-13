---
title: 折腾博客记录
date: 2023-11-13 17:02:27
tags:
---
## 使用cf后无限redirect

在 namesilo 买的域名，突发奇想试试用 Cloudflare 免费计划的域名解析（~~为了CDN~~）

配完后炸了，http 请求返回 301 导致无限 redirect，查了下原因如下：
{% blockquote Vanish <https://vzone.me/897/> %}
服务器端使用了强制HTTPS，CloudFlare 的Flexible策略原理是：用户访问时使用HTTPS访问到CF的节点，然后CF通过HTTP方式回源到你的服务器去读取数据，这个时候对于你的服务器来说，CF就是访客，所以服务器返回的状态都是 301。解决方法很简单，将 SSL 策略设为 Full 或者 Full(strict) 就能解决。
{% endblockquote %}

---

## ~~**上大学上的.jpg**~~

![ddl fuck you](fiddle-with-ddl.jpg)

