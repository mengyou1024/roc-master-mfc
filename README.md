## 最新发布

[https://github.com/mengyou1024/roc-master-mfc/releases/latest](https://github.com/mengyou1024/roc-master-mfc/releases/latest)

### 0. 克隆仓库

``` powershell
git clone <git-repository>
```

### 1. 安装Chocolatey

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

**需要管理员权限**

### 2. 安装依赖包

``` powershell
choco install packages.config
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

