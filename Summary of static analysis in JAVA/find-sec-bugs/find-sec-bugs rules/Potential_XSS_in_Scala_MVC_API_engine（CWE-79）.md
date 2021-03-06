###Potential XSS in Scala MVC API engine

Scala MVC API引擎中潜在的XSS

**Bug Pattern：**SCALA_XSS_MVC_API
####描述：

发现了潜在的XSS。 它可以用来在客户端浏览器中执行不需要的JavaScript。 （见参考资料）

- [x] 安全相关

####漏洞代码：
```
def doGet(value:String) = Action {

    Ok("Hello " + value + " !").as("text/html")

  }

```
####解决方案：
```
def doGet(value:String) = Action {

    Ok("Hello " + Encode.forHtml(value) + " !")

  }

```
针对XSS的最佳防御是上下文敏感的输出编码，如上例所示。 通常有4种上下文需要考虑：HTML，JavaScript，CSS（样式）和URL。 请遵循OWASP XSS预防备忘单中定义的XSS保护规则，这些规则将详细解释这些防范措施。

#### Check级别：AST

####相关信息：
1. [WASC-8: Cross Site Scripting](http://projects.webappsec.org/w/page/13246920/Cross%20Site%20Scripting )
1. [OWASP: XSS Prevention Cheat Sheet ](https://www.owasp.org/index.php/XSS_%28Cross_Site_Scripting%29_Prevention_Cheat_Sheet )

1. [OWASP: Top 10 2013-A3: Cross-Site Scripting (XSS)](https://www.owasp.org/index.php/Top_10_2013-A3-Cross-Site_Scripting_%28XSS%29 )
1. [CWE-79: Improper Neutralization of Input During Web Page Generation ('Cross-site Scripting') ](http://cwe.mitre.org/data/definitions/79.html )
1. [OWASP Java Encoder](https://code.google.com/p/owasp-java-encoder/)







