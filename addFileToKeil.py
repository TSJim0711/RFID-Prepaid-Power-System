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

def add_c_files_only(uvprojx_path, folder_path, group_name_target):
    # 临时重命名以解析 XML
    base, ext = os.path.splitext(uvprojx_path)
    xml_path = base + '.xml'
    os.rename(uvprojx_path, xml_path)

    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()

        # 定位目标分组
        target_group = None
        for group in root.findall('.//Group'):
            gn = group.find('GroupName')
            if gn is not None and gn.text == group_name_target:
                target_group = group
                break

        if target_group is None:
            print(f"❌ 错误：在工程中未找到名为 '{group_name_target}' 的分组。")
            return

        files_node = target_group.find('Files')
        if files_node is None:
            files_node = ET.SubElement(target_group, 'Files')

        # --- 核心修改：维护本次添加历史列表 ---
        added_files_cache = set() 
        new_add_count = 0
        skip_count = 0

        print(f"📂 开始扫描目录: {folder_path}")

        for subdir, _, files in os.walk(folder_path):
            for file in files:
                # 只处理 .c 文件
                if file.endswith('.c'):
                    # 计算相对路径
                    rel_path = os.path.relpath(os.path.join(subdir, file), start=os.path.dirname(xml_path))
                    
                    # 检查本次添加记录
                    if rel_path in added_files_cache:
                        print(f"⚠️  跳过重复文件 (本次已处理): {file}")
                        skip_count += 1
                        continue
                    
                    # 添加到 XML 节点
                    file_node = ET.SubElement(files_node, 'File')
                    ET.SubElement(file_node, 'FileName').text = file
                    ET.SubElement(file_node, 'FileType').text = '1'
                    ET.SubElement(file_node, 'FilePath').text = rel_path
                    
                    # 记录到本次历史
                    added_files_cache.add(rel_path)
                    print(f"✅ 已添加: {file}")
                    new_add_count += 1

        # 保存结果
        indent(root)
        tree.write(xml_path, encoding='utf-8', xml_declaration=True)
        print(f"\n✨ 处理完成！")
        print(f"📝 本次新增: {new_add_count} 个文件")
        print(f"⏭️  本次重复跳过: {skip_count} 个文件")

    except Exception as e:
        print(f"💥 运行出错: {e}")
    finally:
        # 还原工程后缀
        if os.path.exists(xml_path):
            os.rename(xml_path, uvprojx_path)

def find_project():
    projs = glob.glob("*.uvprojx")
    return projs[0] if projs else None

if __name__ == "__main__":
    print("Keil .C 文件快速添加工具 (纯净版)")
    print("-" * 30)
    
    user_input = input("请输入 [目标文件夹路径] [Keil分组名]\n(直接回车使用默认值 ../src Source): ")
    
    if user_input:
        params = user_input.split()
        if len(params) < 2:
            print("参数格式不正确，需要路径和分组名。")
        else:
            path, group = params[0], params[1]
    else:
        path, group = "../src", "Source"

    proj_file = find_project()
    if proj_file:
        add_c_files_only(proj_file, path, group)
    else:
        print("❌ 未在当前目录找到 .uvprojx 工程文件。")