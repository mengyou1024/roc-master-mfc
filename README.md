## 最新发布

[https://github.com/mengyou1024/roc-master-mfc/releases/latest](https://github.com/mengyou1024/roc-master-mfc/releases/latest)

# 本地构建

### 0. 克隆仓库

``` powershell
git clone <git-repository> roc-master
```

### 1. 安装Chocolatey

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

**需要管理员权限**

### 2. 安装依赖包

``` powershell
choco install packages.config -y
# 安装cmake并添加环境变量
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
```

### 3. 编译和打包

#### ①. msbuild 编译(需要msvc命令行环境)

``` powershell
cd roc-master
msbuild Mutilple.sln -t:Mutilple -p:Configuration=Release
```

##### 或者直接使用Visual Studio选择Rlease模式编译

#### ②. 打包安装程序

``` powershell
iscc .\pack-installer.iss
```

# Docker构建

```powershell
# 1. 克隆仓库
git clone <git-repository> roc-master
# 2. 进入目录
cd roc-master
# 3. 构建镜像 (耗时较长)
docker build -t buildtools:latest -m 2GB .
# 4. 进入容器
docker run -it --rm  -v "$(pwd):C:\workdir" buildtools:latest 
# 5. 构建
msbuild Mutilple.sln -t:Mutilple -p:Configuration=Release
# 6. 打包
iscc .\pack-installer.iss
```

