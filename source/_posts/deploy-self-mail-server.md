---
title: 配置自己的邮件服务器
tags:
  - Server
  - Emali
categories:
  - Journal
date: 2024-09-26 12:57:57
---


最近有事需要收发邮件，但是不想用自己的私人邮箱。正好~~手上有个域名，就想着自己搭建一个邮件服务器~~闲得慌，这里记录一下搭建过程。

选用的邮件服务器是 [docker-mailserver](https://docker-mailserver.github.io/docker-mailserver/latest/)，可以方便的在容器内部署，配置也相对简便，更改 `docker-compose.yml` 文件即可。

## 准备工作

### 服务器

首先需要一台服务器，然后装好 docker 和 docker-compose，我使用的是 Debian 12，安装过程不再赘述。

### 域名解析

在你的 DNS 服务商处添加 MX 记录，指向你的服务器地址。我使用 Cloudflare 托管，添加的 MX 记录如下：

```txt
Type: MX
Name: @ // 代表根域名
Content: mail.example.com // 你的邮件服务器地址，这里使用 mail.example.com 作为示例，你可以使用自己的二级域名
Priority: 10 // 优先级，如果只有一个邮件服务器，可以随意设置
Proxied: DNS only
TTL: Auto
```

完成后用 `dig` 命令检查 MX 记录是否生效：

```bash
dig MX example.com
```

### 配置 SSL 证书

docker-mailserver 默认使用 Let's Encrypt 证书，所以需要配置好域名解析，确保能够访问到你的服务器。如果你没有在服务器上部署任何网址(80端口未占用) Let's Encrypt 会自动验证域名，生成证书。

在服务器上，可以使用 `certbot` 工具生成证书，你可以查看[这个网站](https://certbot.eff.org/instructions)来获得在你 WebServer 和发行版上的最佳实现。我的服务器使用的是 Nginx。

```bash
sudo apt install certbot
sudo apt install 
sudo certbot certonly --standalone -d mail.example.com -d example.com
```

### 部署 docker-mailserver

- 首先创建一个目录，用于存放 docker-compose.yml 文件和其他配置文件。

```bash
mkdir mailserver
cd mailserver
```

- 然后将 docker-compose.yml 文件下载到当前目录。

```bash
wget https://raw.githubusercontent.com/docker-mailserver/docker-mailserver/master/docker-compose.yml
```

根据你的需求修改 `docker-compose.yml` 文件，我修改后的文件如下：

```yaml
version: '3.8'

services:
  mailserver:
    image: docker.io/mailserver/docker-mailserver:latest
    hostname: mail.example.com
    domainname: example.com
    container_name: mailserver
    ports:
      - "25:25"
      - "143:143"
      - "465:465"
      - "587:587"
      - "993:993"
    volumes:
      - ./docker-data/dms/mail-data/:/var/mail/
      - ./docker-data/dms/mail-state/:/var/mail-state/
      - ./docker-data/dms/mail-logs/:/var/log/mail/
      - ./docker-data/dms/config/:/tmp/docker-mailserver/
      - /etc/localtime:/etc/localtime:ro
      - /etc/letsencrypt:/etc/letsencrypt
    environment:
      - ENABLE_SPAMASSASSIN=1
      - ENABLE_FAIL2BAN=1
      - ENABLE_OPENDKIM=1
      - ENABLE_OPENDMARC=1
      - POSTFIX_MESSAGE_SIZE_LIMIT=102400000
      - DMS_DEBUG=0
      - SSL_TYPE=letsencrypt
      - SSL_DOMAIN=example.com
      - LETSENCRYPT_EMAIL=admin@example.com
      - SSL_LTS=1                     
    cap_add:
      - NET_ADMIN
    restart: unless-stopped

volumes:
  maildata:
  mailstate:
  maillogs:
```

完成后，运行 `docker-compose up -d` 启动容器。第一次启动后，要在120秒内创建第一个用户，否则容器会自动关闭。

```bash
docker exec -it mailserver setup email add xxx@example.com password
```

### 开放端口

在服务器上开放以下端口：

- 25: SMTP
- 143: IMAP
- 465: SMTPS
- 587: Submission
- 993: IMAPS

我的服务器使用 iptables 防火墙，开放端口的命令如下：

```bash
iptables -A INPUT -p tcp --dport 25 -j ACCEPT
iptables -A INPUT -p tcp --dport 143 -j ACCEPT
iptables -A INPUT -p tcp --dport 465 -j ACCEPT
iptables -A INPUT -p tcp --dport 587 -j ACCEPT
iptables -A INPUT -p tcp --dport 993 -j ACCEPT
```

要持久化这些规则，可以使用 `netfilter-persistent` 工具：

```bash
apt install iptables-persistent
```

执行 `sudo netfilter-persistent save` 保存规则。

## 进阶配置

刚刚配好的邮件服务器给 gmail 发邮件时，会被标记为垃圾邮件，这是因为缺少 SPF、DKIM 和 DMARC 记录。我们需要配置这些记录。

### SPF

SPF 是一种用于防范伪造邮件的技术，通过在 DNS 中添加 SPF 记录，可以指定哪些 IP 地址可以发送邮件。由于我们的邮件服务器只有一台，所以可以直接指定服务器的 IP 地址。在 Cloudflare 中添加 TXT 记录：

```txt
Type: TXT
Name: @
Content: v=spf1 mx -all
TTL: Auto
```

解释：
- `v=spf1`：SPF 版本
- `mx`：允许 MX 记录指定的服务器发送邮件
- `-all`：拒绝所有其他服务器发送邮件

### DKIM

DomainKeys Identified Mail (DKIM) 是一种用于验证邮件来源的技术，通过在 DNS 中添加 DKIM 记录，可以验证邮件的真实性。docker-mailserver 已经为我们内置并自动开启了 OpenDKIM，我们只需要生成 DKIM 密钥并提取公钥添加到 DNS 中。[官方文档](https://docker-mailserver.github.io/docker-mailserver/latest/config/best-practices/dkim_dmarc_spf/#dkim)中有详细的步骤。

由于我们需要指定特定子域名，所以需要在依照文档中 `You may need to specify mail domains explicitly` 部分的步骤中添加 `-d mail.example.com` 参数，命令如下：

```bash
docker exec -it mailserver /bin/bash
setup config dkim domain 'mail.example.com'
setup config dkim domain mail.example.com
```

然后，在刚刚打开的 shell 中查看公钥：

```bash
cat /tmp/docker-mailserver/opendkim/keys/mail.example.com/mail.txt
```

将输出的内容添加到 DNS 中，具体参考[这块](https://docker-mailserver.github.io/docker-mailserver/latest/config/best-practices/dkim_dmarc_spf/#dkim-dns)：

```txt
Type: TXT
Name: mail._domainkey
Content: v=DKIM1; k=rsa; p=xxxxxx
TTL: Auto
```

### DMARC

Domain-based Message Authentication, Reporting and Conformance (DMARC) 是一种用于验证邮件来源的技术，通过在 DNS 中添加 DMARC 记录，可以指定邮件的处理方式。docker-mailserver 已经为我们内置并自动开启了 OpenDMARC，我们只需要添加 DMARC 记录。在 Cloudflare 中添加 TXT 记录：

```txt
Type: TXT
Name: _dmarc
Content: v=DMARC1; p=none; rua=mailto:admin@example.com
TTL: Auto
```

解释：

- `v=DMARC1`：DMARC 版本
- `p=none`：指定邮件的处理方式，`none` 表示不做任何处理
- `rua=mailto:`：指定报告的接收邮箱
- `ruf=mailto:`：指定失败报告的接收邮箱
- `sp=none`：指定子域名的处理方式
- `adkim=s`：指定 DKIM 的验证方式
- `aspf=r`：指定 SPF 的验证方式
- `pct=100`：指定报告的百分比
- `rf=afrf`：指定报告的格式

### rDNS

反向 DNS (rDNS) 是一种用于验证 IP 地址的技术，通过在 DNS 中添加 PTR 记录，可以验证 IP 地址的真实性。这需要联系服务器提供商来配置。

## 验证邮件服务器正常工作

我们可以使用 [dkimvalidator.com](https://dkimvalidator.com/) 来验证 DKIM 和 DMARC 记录是否生效，使用 [www.mail-tester.com](https://www.mail-tester.com/) 来验证邮件是否被标记为垃圾邮件。