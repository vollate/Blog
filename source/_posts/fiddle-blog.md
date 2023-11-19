---
title: 折腾博客记录
date: 2023-11-13 17:02:27
tags:
- Fiddle
categories:
- Network
---
原来的 vps 到期了，不想续费新开了一台，忘了全系统备份了。索性新 vps 改成 Debian，blog 扔 github page,然后又是一顿折腾。

## Github Action 样例

改了改 Hexo 官网的:

```yaml
name: Blog

on:
  push:
    branches:
      - main
jobs:
  pages:
    runs-on: ubuntu-latest
    permissions:
      contents: write
    steps:
      - uses: actions/checkout@v3
        with:
          token: ${{ secrets.GITHUB_TOKEN }}
          submodules: true   
      - name: Use Node.js 18.x
        uses: actions/setup-node@v2
        with:
          node-version: "18"
      - name: Install Dependencies
        run: npm install
      - name: Build
        run: npm run build
      - name: Deploy
        uses: peaceiris/actions-gh-pages@v3
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./public
```

## Github Action 禁用 jekyll build

因为要用 `peaceiris/actions-gh-pages@v3` 进行部署，但是用的 Hexo 且已经在先前的操作编译完成，只需部署到对应分支即可。

~~一开始不知道 debug 了半天~~ 解决方法在仓库里创建一个`.nojekyll`文件即可, 这会自动禁用 jekyll build 而只执行 deploy (部署编译好的文件到对应分支)

## 使用cf后无限redirect

在 namesilo 买的域名，突发奇想试试用 Cloudflare 免费计划的域名解析（~~为了CDN~~）

配完后炸了，http 请求返回 301 导致无限 redirect，查了下原因如下：
{% blockquote Vanish  https://vzone.me/897/ %}
服务器端使用了强制HTTPS，CloudFlare 的Flexible策略原理是：用户访问时使用HTTPS访问到CF的节点，然后CF通过HTTP方式回源到你的服务器去读取数据，这个时候对于你的服务器来说，CF就是访客，所以服务器返回的状态都是 301。解决方法很简单，将 SSL 策略设为 Full 或者 Full(strict) 就能解决。
{% endblockquote %}

---

## ~~**上大学上的.jpg**~~

![ddl fuck you](fiddle-with-ddl.jpg)

