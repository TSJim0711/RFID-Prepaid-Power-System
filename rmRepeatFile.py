import os
import glob
import xml.etree.ElementTree as ET

def indent(elem, level=0):
    """ 格式化 XML 缩进 """
    i = "\n" + level * "    "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "    "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for subelem in elem:
            indent(subelem, level + 1)
        if not subelem.tail or not subelem.tail.strip():
            subelem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
        if not elem.tail:
            elem.tail = "\n"

def remove_duplicate_files(uvprojx_path):
    # 临时更改扩展名
    base, ext = os.path.splitext(uvprojx_path)
    xml_path = base + '.xml'
    os.rename(uvprojx_path, xml_path)

    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()

        # 用于存储已发现的文件路径： key=文件相对路径, value=所属第一个分组名
        seen_files = {}
        removed_count = 0

        print(f"🔍 正在检查工程: {uvprojx_path}")

        # 遍历所有 Group
        for group in root.findall('.//Group'):
            group_name = group.find('GroupName').text if group.find('GroupName') is not None else "Unknown Group"
            files_node = group.find('Files')
            
            if files_node is None:
                continue

            # 遍历当前 Group 下的所有 File 节点
            # 使用 list() 是为了在循环中安全删除元素
            for file_node in list(files_node.findall('File')):
                path_node = file_node.find('FilePath')
                if path_node is None or not path_node.text:
                    continue
                
                # 获取标准化的路径名（统一斜杠并去除空格）
                file_path = path_node.text.strip().replace('\\', '/')
                
                if file_path in seen_files:
                    # 发现重复！从当前 Files 节点中移除该 File 节点
                    files_node.remove(file_node)
                    print(f"✂️  移除重复: [{file_path}] (已存在于分组: {seen_files[file_path]})")
                    removed_count += 1
                else:
                    # 第一次见到，记录下来
                    seen_files[file_path] = group_name

        if removed_count > 0:
            # 只有在有变动时才保存
            indent(root)
            tree.write(xml_path, encoding='utf-8', xml_declaration=True)
            print(f"\n✨ 清理完成！共移除 {removed_count} 个跨分组重复文件。")
        else:
            print("\n✅ 工程很干净，未发现跨分组重复文件。")

    except Exception as e:
        print(f"💥 运行出错: {e}")
    finally:
        # 还原工程后缀
        if os.path.exists(xml_path):
            os.rename(xml_path, uvprojx_path)

if __name__ == "__main__":
    # 查找当前目录下的工程文件
    projs = glob.glob("*.uvprojx")
    if not projs:
        print("❌ 未找到 .uvprojx 工程文件。")
    else:
        print("--- Keil 工程跨分组去重工具 ---")
        remove_duplicate_files(projs[0])