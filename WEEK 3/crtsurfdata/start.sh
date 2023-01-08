#启动数据中心后台服务程序脚本


#检查服务是否超时
#/root/project/tools1/bin/procctl 30 /root/project/tools1/bin/checkproc

#压缩数据中心后台程序备份日志
/root/project/tools1/bin/procctl 300 /root/project/tools1/bin/gzipfiles /root/log/idc ".log.20*" 0.04

#生成用于测试的全国气象站点每分钟观测数据。
/root/project/tools1/bin/procctl 60 /root/project/idc1/bin/crtsurfdata /root/project/idc1/ini/stcode.ini /root/tmp/idc/surfdata /root/log/idc/crtsurfdata.log xml,json,csv

#清理原始的全国气象站点每分钟数据目录
/root/project/tools1/bin/procctl 300 /root/project/tools1/bin/deletefiles /root/tmp/idc/surfdata "*" 0.04
