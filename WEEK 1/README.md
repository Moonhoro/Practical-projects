# 前期配置
1. 安装虚拟机 操作系统CentOS7
2. 安装MySQL

# 本周主要内容
1. crtsurfdata1中主要写了基本框架，对日志文件生成进行了测试

2. crtsurfdata2中将全国气象站点信息加载到vstcode容器中。

3. crtsurfdata3中利用随机数生成测试数据并放入vsurfdata容器中。

4. crtsurfdata4中实现将vsurfdata容器中的数据以csv形式储存。

5. crtsurfdata5中实现将vsurfdata容器中的数据以json、xml形式储存。

# 储存形式
1. csv：第一行为列名或不写列名，每个字段以,分隔。

2. xml：以```<A></A>```分隔

3. json以"":"",的格式分隔，各组数据之间以{}，分隔，开头结尾处为{[]}
