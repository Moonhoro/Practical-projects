因为考试原因，中间断了，一直没有上传，现在才发现，重新传一下。

xmltodb.cpp 把xml文件导入mysql数据库（当时搞Oracle数据库，这个让改的乱七八糟的，懒得搞了就这样吧）

syncupdate.cpp 采用刷新的办法同步MySQL数据库之间的表

syncincrement.cpp采用增量的方式同步MySQL数据库之间的表(采用federated引擎)

syncincrementex.cpp采用增量的方式同步MySQL数据库之间的表(不采用federated引擎)

deletetable.cpp表中数据清理

deletetable.cpp表中数据迁移
