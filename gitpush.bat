@echo off
cd /d %~dp0  :: 自动切换到当前批处理文件所在目录
set commit_message=Auto commit %DATE% %TIME%
git add .
git commit -m "%commit_message%"
git push
pause
