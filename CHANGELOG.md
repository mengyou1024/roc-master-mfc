# v1.0.231225 - 2023-12-25

## 🎉 New Features
- 添加单行输入框的正则校验以及滚轮事件
- 
- 可执行文件更名为Roc-Master
- 添加波门计算结果的显示和测厚结果的显示
- 调整波门跟随的逻辑、删除软件的管理员权限请求、添加波门数据的显示

## 🐛 Bugfixes
- wchar_t会隐式转换为bool, 无法转换为wstring_view
- 缺少分号导致应用图标丢失
- 设置声程太高时A扫卡死,采样因子1-127实际软件中可设置范围为0-255
- 修复Edit某些情况无法输入的bug

## 🔨 Improvements
- CEditUI添加正则校验器
- CEditUI添加针对数值功能, 添加	extvalitor、modenumber、
umberminmax、wheelstep属性
- cmake中添加对dll的搜索，不再需要手动添加dll到打包脚本

## 🧱 Build System
- 发布assert携带版本信息
- 添加changelog的推送
- changelog的换行信息丢失

## 🪚 Refactors
- 修改部分文件名



