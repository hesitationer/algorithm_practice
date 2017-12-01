# -*- coding: utf-8 -*-

import glob

code_base = "E:\JC_New/**/*.cpp"

def get_all_cpp_files():
	f_list = [x for x in glob.glob(code_base, recursive=True) if
	("GGJCalculator"  in x or "GGJCommonData" in x) and ("moc_" not in x)]
	with open("../Release/all_cpp.txt", 'w+') as f:
		f.write("\n".join(f_list))

get_all_cpp_files();	
