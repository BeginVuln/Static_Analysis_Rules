###Object deserialization is used

使用对象反序列化

**Bug Pattern：**OBJECT_DESERIALIZATION
####描述：
如果类路径中存在允许触发恶意操作的类，则不可信数据的对象反序列化可能会导致远程代码执行。



图书馆开发人员倾向于修复提供潜在恶意触发的类。 还有一些已知触发拒绝服务的类[1]。



反序列化是一个明智的操作，具有很大的漏洞史。 一旦在Java虚拟机[2] [3]中发现新的漏洞，Web应用程序可能会变得易受攻击。

- [x] 安全相关

####风险代码：
```
public UserData deserializeObject(InputStream receivedFile) throws IOException, ClassNotFoundException {



    try (ObjectInputStream in = new ObjectInputStream(receivedFile)) {

        return (UserData) in.readObject();

    }

}

```
#### Check级别：AST

####解决方案：
Avoid deserializing object provided by remote users.

####相关信息：
[CWE-502: Deserialization of Untrusted Data]( https://cwe.mitre.org/data/definitions/502.html)
[Deserialization of untrusted data]( https://www.owasp.org/index.php/Deserialization_of_untrusted_data)
[Serialization and Deserialization ]( http://www.oracle.com/technetwork/java/seccodeguide-139067.html#8)
[A tool for generating payloads that exploit unsafe Java object deserialization]( https://github.com/frohoff/ysoserial)
[[1] Example of Denial of Service using the class java.util.HashSet]( https://gist.github.com/coekie/a27cc406fc9f3dc7a70d)
[[2] OpenJDK: Deserialization issue in ObjectInputStream.readSerialData() (CVE-2015-2590)]( https://bugzilla.redhat.com/show_bug.cgi?id=CVE-2015-2590)
[[3] Rapid7: Sun Java Calendar Deserialization Privilege Escalation (CVE-2008-5353)](https://www.rapid7.com/db/modules/exploit/multi/browser/java_calendar_deserialize)


