###Persistent Cookie Usage
持久Cookie使用
**Bug Pattern：**COOKIE_PERSISTENT
####描述：
将敏感数据长时间存储在持久性cookie中可能会导致违反机密或帐户受损。

####进一步解释：

如果私人信息存储在持久性cookie中，攻击者就有更大的时间窗口窃取这些数据 - 尤其是因为永久性cookie通常会在不久的将来失效。 持久性cookies通常存储在客户端的文本文件中，并且可以访问受害者机器的攻击者可以窃取这些信息。

持久性Cookie通常用于在用户与网站互动时对其进行配置。 根据跟踪数据的处理方式，可能会使用永久性Cookie来侵犯用户的隐私。

- [x] 安全相关

####漏洞代码：
```
[...]

Cookie cookie = new Cookie("email", email);

cookie.setMaxAge(60*60*24*365);

[...]

```
####解决方案：

Use persistent cookies only if necessary and limit their maximum age.

Don't use persistent cookies for sensitive data.


#### Check级别：AST

####相关信息：
1. [CWE-539: Information Exposure Through Persistent Cookies]( https://cwe.mitre.org/data/definitions/539.html)
2. [Class Cookie setMaxAge documentation](https://tomcat.apache.org/tomcat-5.5-doc/servletapi/javax/servlet/http/Cookie.html#setMaxAge%28int%29)


