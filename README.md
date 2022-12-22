# Sudoku
## 概述：
大二数据结构课程设计  
本项目使用C++，基于EasyX共设计14个类，累计1760余行代码，完成了简单数独游戏的编写.
## 前言
本程序为方便调试和读者快速体验全流程，故默认的游戏初局中只有1个空格未填，若想修改默认初局，只需更改**MapMain**矩阵
## 操作说明
鼠标左键点击
## 系统功能设计：
1.用户名录入  
初始界面录入玩家用户名，方便后续记录得分及排序  
2.关卡选项  
可选择直接开始游戏（默认为第一关）或者选择关卡  
3.导入关卡  
通过读取文档（记录了多种初局）来设置关卡  
4.计时器  
记录玩家游玩时间，最后换算得分  
5.数字选择  
用鼠标在游戏界面中选择数字填入  
6.选择关卡  
通过点击对应按钮选择对应初局  
7.九宫格  
绘制数独，并包含有高亮关键数字等功能  
8.游戏过程选项  
包含是否暂停游戏、重新开始、直接退出和显示答案等功能  
9.记录玩家得分  
通过游戏用时来计算玩家得分，并存入文档  
10.玩家得分排序  
通过对玩家的得分进行比较来排序，可切换不同榜单  
## 系统功能图

![系统功能设计](http://m.qpic.cn/psc?/V50PQLrG21HPuk2xuTDP21m7W34eYMMH/bqQfVz5yrrGYSXMvKr.cqeCqjFvQ8RoJ0WjBZRhRZqxM79abukuQkl1ExE.JbqCf2uOwYyTB5MrjG6*BvErgEoMexp4fnL326zM00Q3fgo8!/b&bo=5wTVAgAAAAADBxY!&rf=viewer_4 "系统功能设计")
## 预览
![开始界面](http://m.qpic.cn/psc?/V50PQLrG21HPuk2xuTDP21m7W34eYMMH/bqQfVz5yrrGYSXMvKr.cqdwNfJjM9CKVHLLn9KpAJKndKKH6fSnnI6hLYclWn5QskFM1BZLMGdYjf1TsS9MKssvPgBoVjJLQwUlZfPbjGwU!/b&bo=*AcbBAAAAAADJ.Y!&rf=viewer_4 "开始界面")
![关卡选择界面](http://m.qpic.cn/psc?/V50PQLrG21HPuk2xuTDP21m7W34eYMMH/bqQfVz5yrrGYSXMvKr.cqQUY4SfFNkTiD4zG*rFIQXiAV4lVsNX1h19N4CZpAaGBQYnN.uYmARCw*R2YyDwE91SOGUvZYpK21Zb29ddFcwU!/b&bo=*AcbBAAAAAADN*Y!&rf=viewer_4 "关卡选择界面")
![游戏主界面](http://m.qpic.cn/psc?/V50PQLrG21HPuk2xuTDP21m7W34eYMMH/bqQfVz5yrrGYSXMvKr.cqXHUJae5nN1EZz32hByauHSZIl3wf20HRkQWLdqhypcoUvS3yAcTQBZB0xnU4NI7jqp0VrhjSQRXYEfZshcja14!/b&bo=*AcbBAAAAAADN*Y!&rf=viewer_4 "游戏主界面")
![结束界面](http://m.qpic.cn/psc?/V50PQLrG21HPuk2xuTDP21m7W34eYMMH/bqQfVz5yrrGYSXMvKr.cqekeW8O0MmLPOONTbZdzYPeX2jj96N41LLVcJBkQFwb0oFeYiNg3N6LQ*fERPLp5R20yRiX*O1lS27hLswPdfAw!/b&bo=*AcbBAAAAAADR4Y!&rf=viewer_4 "结束界面")
## 备注
数独.exe 为打包好的可执行文件  
Demo\Icon 为游戏内插图文件夹  
Demo\ScoreTable.txt 为游戏历史记录  
Demo\sudoku.txt 为游戏关卡库文件  
