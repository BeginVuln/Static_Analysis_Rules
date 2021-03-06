###SMTP Header Injection
**Bug Pattern:**SMTP_HEADER_INJECTION
[CWE-93: Improper Neutralization of CRLF Sequences ('CRLF Injection')]( https://cwe.mitre.org/data/definitions/93.html)
####描述：
简单邮件传输协议（SMTP）是用于电子邮件传递的基于文本的协议。 与HTTP一样，头部由新行分隔符分隔。 如果用户输入位于头部中，应用程序应将其删除或替换为一个新行字符（CR / LF）。 你应该使用一个安全的包装器，例如Apache Common Email和Simple Java Mail来过滤可能导致头部注入的特殊字符。

- [x] 安全相关 

#### 漏洞代码：

```

Message message = new MimeMessage(session);

message.setFrom(new InternetAddress("noreply@your-organisation.com"));

message.setRecipients(Message.RecipientType.TO, new InternetAddress[] {new InternetAddress("target@gmail.com")});

message.setSubject(usernameDisplay + " has sent you notification"); //Injectable API

message.setText("Visit your ACME Corp profile for more info.");

Transport.send(message);

```
####解决方案：
使用Apache Common Email or Simple Java Mail.
#### check级别: AST,data-flow 

####相关信息：
1. [OWASP SMTP Injection]( https://www.owasp.org/index.php/Testing_for_IMAP/SMTP_Injection_(OTG-INPVAL-011))
2. [Commons Email: User Guide]( https://commons.apache.org/proper/commons-email/userguide.html)
3. [Simple Java Mail Website]( http://www.simplejavamail.org/)
4. [StackExchange InfoSec: What threats come from CRLF in email generation?](https://security.stackexchange.com/a/54100/24973)



