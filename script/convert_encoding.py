from pathlib import Path
import chardet
import re


def text_file_encoding_convert(f: Path, target_encoding: str, *, dry_run=False) -> (bool, str, float):
    ''' 转换单个文件到目标编码
    @param  f                   文件路径
    @param  target_encoding     目标编码，比如urf-8
    @param  dry_run             为True 时不实际修改源文件

    @return  返回三个值分别为（是否成功，估计的源文件编码，估计的把握）
    '''
    target_encoding = target_encoding.lower()    # python 的标准编码名称都是小写
    raw = f.read_bytes()
    result = chardet.detect(raw)
    if (result["encoding"] is None):
        return (False, "", 0.0)
    encoding = result["encoding"].lower()     # chardet 估计出来的编码名称
    confidence = result["confidence"]    # 估计的把握
    flag = True

    # 下面的单次for 循环用来避免重复写return 语句，break 后统一跳到最后return
    for _db_ in (1,):
        if encoding == target_encoding or encoding == "ascii" and target_encoding == "utf-8":
            # 目标编码和源编码相同时不用做处理。utf-8 编码和ASCII 兼容，原编码为ASCII 时转换到utf-8 不会有变化，所以也跳过
            print(
                f"-> [NO CONVERSION NEEDED] {f.name: ^32}: {encoding} ==> [ {target_encoding} ]")
            break

        try:
            text = raw.decode(encoding)
        except:
            print(
                f"!> Encoding err: {f.name: ^32}, detected: {encoding}, {confidence}.")
            flag = False
            break

        if dry_run:
            print(
                f"-> [ NO WET ] {f.name: ^32}: {encoding} ==> [ {target_encoding} ]")
        else:
            # 必须先用目标编码转换成字节数组，然后按字节写入源文件
            # 如果按文本方式写入，就会遇到喜闻乐见的CR LF 换行问题，
            # 源文件中的CR LF 换行会被自动变成CR CR LF，也就是多了一堆空行。
            out = text.encode(target_encoding)
            f.write_bytes(out)
            print(f"-> {f.name: ^32}: {encoding} ==> [ {target_encoding} ]")

    return (flag, encoding, confidence)


def text_file_encoding_batch_convert(
    folder: Path,
    target_encoding: str,
    *,
    dry_run=True,
    recursive=False,
    pattern=".*\.(h|(hpp)|(hxx)|c|(cpp)|(cxx))$",
    skip_when_error=True,
):
    ''' 批量转换一个目录下文本文件的编码
    @param  folder             目标目录
    @param  target_encoding    目标编码
    @param  dry_run            不实际修改源文件，避免手滑写错
    @param  recursive          包括所有子文件夹下的文件
    @param  pattern            基于文件名筛选文本文件的正则表达式，默认根据后缀筛选几种文本类型
    @param  skip_when_error    默认True，单个文件转换出错时提示并跳过，否则终止
    '''
    if recursive:
        flist = folder.rglob("*")
    else:
        flist = folder.glob("*")

    p = re.compile(pattern)   # 把正则编译了，之后应该能快一点
    for f in flist:
        if not (f.is_file() and p.match(f.name)):
            continue

        ok, encoding, confidence = text_file_encoding_convert(
            f, target_encoding, dry_run=dry_run)
        if not ok:
            if skip_when_error:
                print(f"-> [SKIP] {f.name: ^32}")
                print("!> SKIP.")
            else:
                print("!> ABORT.")
                return


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="批量转换一个目录下文本文件的编码")
    parser.add_argument("folder", type=str, help="目标目录")
    parser.add_argument("target_encoding", type=str, help="目标编码")
    parser.add_argument("--dry-run", action="store_true",
                        help="不实际修改源文件，避免手滑写错")
    parser.add_argument("--recursive", action="store_true",
                        help="包括所有子文件夹下的文件")
    parser.add_argument("--pattern", type=str, default=".*\.(h|(hpp)|(hxx)|c|(cpp)|(cxx))$",
                        help="基于文件名筛选文本文件的正则表达式，默认根据后缀筛选几种文本类型")
    parser.add_argument("--skip-when-error", action="store_true",
                        help="默认True，单个文件转换出错时提示并跳过，否则终止")
    args = parser.parse_args()
    text_file_encoding_batch_convert(
        Path(args.folder),
        args.target_encoding,
        dry_run=args.dry_run,
        recursive=args.recursive,
        pattern=args.pattern,
        skip_when_error=args.skip_when_error,
    )
    print("Done.")
