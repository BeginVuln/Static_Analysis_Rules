###URL rewriting method
URL重写方法
**Bug Pattern：** URL_REWRITING
####描述：
此方法的实现包括确定会话ID是否需要在URL中进行编码的逻辑。

URL重写存在重大的安全风险。 由于会话ID出现在URL中，因此可能很容易被第三方看到。 URL中的会话ID可以用多种方式公开，例如：

日志文件，

浏览器的历史，

通过将其复制并粘贴到电子邮件或发布中，

HTTP推荐人。

- [x] 安全相关

####漏洞代码：
```
out.println("Click <a href=" +

                res.encodeURL(HttpUtils.getRequestURL(req).toString()) +

                ">here</a>");

```
####解决方案：
Avoid using those methods. If you are looking to encode a URL String or form parameters do not confuse the URL rewriting methods with the URLEncoder class.

#### Check级别：AST

####相关信息：
1. [OWASP Top 10 2010-A3-Broken Authentication and Session Management](https://www.owasp.org/index.php/Top_10_2010-A3-Broken_Authentication_and_Session_Management)


