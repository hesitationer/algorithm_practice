# -*- coding=utf-8 -*-
import numpy as np

# per month ,
year_interest = 0.0655
month_interest = year_interest / 12

income = 6752 + 3000

loan_a = 3870.97 #30-years
delta_loan_a = 470000/(30*12)*month_interest

loan_ab = 2986.19

daily_expense = 3000
gjj = 650

#每月剩余的钱
diff_ab = np.zeros((30*12,),dtype=np.float)
diff_a = np.zeros((30*12,),dtype=np.float)

for i in range(30*12):
	diff_ab[i] =  income - daily_expense + gjj - loan_ab
	diff_a[i] =  income - daily_expense + gjj - (loan_a-i*delta_loan_a)

print diff_ab[:24]
print diff_a[:24]

#6月内能攒的钱
print sum(diff_ab[:6])
print sum(diff_a[:6])

#6-12个月内能攒的钱
print sum(diff_ab[6:6+6])
print sum(diff_a[6:6+6])
