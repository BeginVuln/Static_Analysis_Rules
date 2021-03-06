###JavaBeans Property Injection
属性设置时导致的注入攻击
**Bug Pattern：** BEAN_PROPERTY_INJECTION
####描述：
攻击者可以设置可能危害系统完整性的任意bean属性。 Bean群体函数允许设置一个bean属性或一个嵌套属性。 攻击者可以利用这个功能来访问像class.classLoader这样的特殊bean属性，这将允许他覆盖系统属性并可能执行任意代码。

- [x] 安全相关

####漏洞代码：
```java
MyBean bean = ...;

HashMap map = new HashMap();

Enumeration names = request.getParameterNames();

while (names.hasMoreElements()) {

    String name = (String) names.nextElement();

    map.put(name, request.getParameterValues(name));

}

BeanUtils.populate(bean, map);

```
####解决方案：
Avoid using user controlled values to populate Bean property names.

#### Check级别：AST

####相关信息：
1. [CWE-15: External Control of System or Configuration Setting](https://cwe.mitre.org/data/definitions/15.html)


