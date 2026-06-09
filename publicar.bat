@echo off
chcp 65001 >nul
setlocal

cd /d "%~dp0"
title Publicar Inventario no GitHub

echo.
echo ========================================
echo   Publicar no GitHub - Bolsa Devoradora
echo ========================================
echo.

git --version >nul 2>&1
if errorlevel 1 (
    echo [ERRO] Git nao encontrado. Instale em: https://git-scm.com
    goto fim
)

git rev-parse --is-inside-work-tree >nul 2>&1
if errorlevel 1 (
    echo [ERRO] Esta pasta nao e um repositorio Git.
    goto fim
)

git remote get-url origin >nul 2>&1
if errorlevel 1 (
    echo Configurando remote origin...
    git remote add origin https://github.com/RicardoPalhares/inventario.git
) else (
    git remote set-url origin https://github.com/RicardoPalhares/inventario.git
)

REM Tira arquivos ignorados do controle de versao (se ja estavam adicionados)
git rm --cached -f inventario.exe inventario_test.exe main 2>nul
for %%f in (*.zip) do git rm --cached -f "%%f" 2>nul

git diff --name-only --diff-filter=U 2>nul | findstr /r "." >nul
if not errorlevel 1 (
    echo.
    echo [ERRO] Ha conflitos de merge pendentes.
    echo Resolva os arquivos marcados e rode este .bat de novo.
    goto fim
)

git add -A

git diff --cached --quiet
if not errorlevel 1 (
    git diff --quiet
    if not errorlevel 1 (
        echo Nada novo para publicar.
        goto fim
    )
)

for /f "tokens=1-3 delims=/ " %%a in ("%date%") do set DATA=%%c-%%b-%%a
for /f "tokens=1-2 delims=: " %%a in ("%time%") do set HORA=%%a:%%b
set HORA=%HORA: =0%

if exist ".git\MERGE_HEAD" (
    echo Finalizando merge...
    git commit --no-edit
    if errorlevel 1 (
        git commit -m "Merge com remoto %DATA% %HORA%"
    )
) else (
    echo Criando commit...
    git commit -m "Atualizacao %DATA% %HORA%"
)

if errorlevel 1 (
    git diff --cached --quiet
    if not errorlevel 1 (
        echo Nada novo para publicar.
        goto fim
    )
    echo [ERRO] Falha ao criar commit.
    echo Verifique se o Git ja tem seu nome e e-mail configurados.
    goto fim
)

git branch -M main

echo.
echo Sincronizando com o remoto...
git fetch origin 2>nul
git rev-parse origin/main >nul 2>&1
if not errorlevel 1 (
    git pull origin main --no-edit 2>nul
    if errorlevel 1 (
        git pull origin main --rebase --autostash 2>nul
    )
)

git diff --name-only --diff-filter=U 2>nul | findstr /r "." >nul
if not errorlevel 1 (
    echo.
    echo [ERRO] Conflito ao sincronizar com o remoto.
    echo Resolva no editor e rode este .bat de novo.
    goto fim
)

echo Enviando para https://github.com/RicardoPalhares/inventario ...
git push -u origin main
if errorlevel 1 (
    echo.
    echo [ERRO] Falha no push.
    echo - Feche programas que estejam usando arquivos do projeto
    echo - Confirme login no GitHub ^(Git Credential Manager ou gh auth login^)
    echo - Se aparecer conflito, resolva no VS Code/Cursor e rode este .bat de novo
    goto fim
)

echo.
echo [OK] Publicado com sucesso!

:fim
echo.
pause
endlocal
