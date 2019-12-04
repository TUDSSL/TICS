# MIT License
#
# Copyright (c) 2019 TU Delft Embedded and Networked Systems Group/
# Sustainable Systems Laboratory.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


# Script used in generating results for the following paper
#
# @inproceedings{kortbeek_asplos2020,
#     author = "Vito {Kortbeek} and Kasim Sinan {Yildirim} and Abu {Bakar} and Jacob {Sorber}
#     and Josiah {Hester} and Przemys{\l}}aw {Pawe{\l}czak}",
#     title = "Time-sensitive Intermittent Computing Meets Legacy Software",
#     year = "2020",
#     booktitle = "Proc. ASPLOS",
#     address = "Lausanne, Switzerland",
#     month = mar  # " 16--20,",
#     publisher = "ACM"
# }

from pathlib import Path
from scipy import stats
import pandas as pd
import re
import matplotlib.pyplot as plt
import numpy as np
import sys


# Read file of survey results
results_folder = Path("../user-study-results/surveygizmo")
results_file = results_folder / "20190109034400-SurveyExport_anonymized.csv" # results used in [kortbeek_asplos2020]
results_data = pd.read_csv(results_file, keep_default_na=False) # replace empty fields with "-"
#results_data = results_data[results_data['Status'] == 'Complete'] # Remove incomplete responses

print('---------------------') # program begin marker

try:
    results_data['Extended Referer']
except:
    # Surveygizmo CSV export does not contain "Extended Referer" and "Extended User Agent" fields after 4 December 2019
    # add these here, as columns are addressed by numbers, not by names later on
    results_data.insert(8, "Extended Referer", results_data['Referer'])
    results_data.insert(12, "Extended User Agent", results_data['User Agent'])

# Read file of MTurk results
# Note: all results - fron MTurk and non-MTurk users were used in ASPLOS 2020 paper [kortbeek_asplos2020];
# feel free to experiment by chosing different uses cohorts
mturk_folder = Path("../user-study-results/mturk")
mturk_file = mturk_folder / "Batch_3492166_batch_results_anonymized.csv"
mturk_data = pd.read_csv(mturk_file, keep_default_na=False) # replace empty fields with "-"

# Use only MTurk results
# results_data = results_data[results_data['Confirmation code'].isin(list(set(mturk_data['Answer.surveycode'])))]

# Exclude MTurk results
# results_data = results_data[~results_data['Confirmation code'].isin(list(set(mturk_data['Answer.surveycode'])))]

# General stats
no_responses = len(results_data.iloc[0:,1]) # pick any random column to measure number of respondents

print("No. responses:", no_responses)
print("")

# Find information about countries of respondents
unique_countries = sorted(list(set(results_data['Country'].sort_values())))
no_unique_countries = len(unique_countries)

print("Countries:", unique_countries)
print("No. Countries:", no_unique_countries)
print("")

# Find information about cities of respondents
unique_cities = sorted(list(set(results_data['City'].sort_values())))
no_unique_cities = len(unique_cities)

print("Cities:", unique_cities)
print("No. Cities:", no_unique_cities)
print("")

ind_time_explanation_noutl = results_data[results_data['Time spend on explanation'] <
                                results_data['Time spend on explanation'].mean() +
                                3 * results_data['Time spend on explanation'].std()].index.tolist()

# Find information about time spent on reading `Explanation` section of the survey
time_explanation_avg = results_data['Time spend on explanation'][ind_time_explanation_noutl].mean()/60
time_explanation_std = results_data['Time spend on explanation'][ind_time_explanation_noutl].std()/60
time_explanation_min = results_data['Time spend on explanation'][ind_time_explanation_noutl].min()/60
time_explanation_max = results_data['Time spend on explanation'][ind_time_explanation_noutl].max()/60

print("Avg Time Spent on `Explanation` (s):", time_explanation_avg)
print("Std Time Spent on `Explanation` (s):", time_explanation_std)
print("")

# Find information about finding bug in `swap' program with `checkpoints`

correct_line_swap_c = 'Line 9' # correct line for `swap' program with `checkpoints`
line_swap_c = results_data.iloc[0:,23] # address by column number, not by name - name is too long
# (Note: works for surveygizmo exports before 4 December 2019 -
# "Extended referer" and "Extended user agent" column does not exist any more)

no_correct_line_swap_c = len([x for x in list(line_swap_c) if correct_line_swap_c in x]) # number of correct responses

print("No. Correct Responses (Swap/Chechpoint):", no_correct_line_swap_c)
print("Correct Responses (Swap/Chechpoint) (%):", no_correct_line_swap_c / no_responses * 100)

correct_expr_swap_c = 'a=a-b' # correct expression for `swap' program with `checkpoints`
expr_swap_c = list(results_data.iloc[0:,24]) # address by column number, not by name - name is too long
# (Note: see comment for line_swap_c eariler)

no_correct_expr_swap_c = 0
for i in range(0, len(expr_swap_c)):
  expr_swap_c[i] = expr_swap_c[i].replace(' ', '') # remove spaces
  expr_swap_c[i] = expr_swap_c[i].replace(';', '') # remove ";" (we assume to accept missing ";")
  if bool(re.search(correct_expr_swap_c, expr_swap_c[i])):
      no_correct_expr_swap_c = no_correct_expr_swap_c + 1

print("No. Correct Expressions (Swap/Chechpoint):", no_correct_line_swap_c)
print("Correct Expressions (Swap/Chechpoint) (%):", no_correct_line_swap_c / no_responses * 100)

# Find all values except outlier
ind_swap_c_noutl = results_data[results_data['Time spent swap checkpoint'] <
                                results_data['Time spent swap checkpoint'].mean() +
                                3 * results_data['Time spent swap checkpoint'].std()].index.tolist()

time_swap_c_avg = results_data['Time spent swap checkpoint'][ind_swap_c_noutl].mean()/60
time_swap_c_std = results_data['Time spent swap checkpoint'][ind_swap_c_noutl].std()/60
time_swap_c_min = results_data['Time spent swap checkpoint'][ind_swap_c_noutl].min()/60
time_swap_c_max = results_data['Time spent swap checkpoint'][ind_swap_c_noutl].max()/60

print("Avg Time Spent on `Swap/Chechpoint` (s):", time_swap_c_avg)
print("Std Time Spent on `Swap/Chechpoint` (s):", time_swap_c_std)
print("")

# # Find information about finding bug in `swap' program with `tasks`

correct_line_swap_t = 'Line 25' # correct line for `swap' program with `tasks`
line_swap_t = results_data.iloc[0:,26] # address by column number, not by name - name is too long
# (Note: see comment for line_swap_c eariler)
no_correct_line_swap_t = len([x for x in list(line_swap_t) if correct_line_swap_t in x]) # number of correct responses

print("No. Correct Responses (Swap/Task):", no_correct_line_swap_t)
print("Correct Responses (Swap/Task) (%):", no_correct_line_swap_t / no_responses * 100)

correct_expr_swap_t = 'SET\(a,GET\(a\)-GET\(b\)\)' # correct expression for `swap' program with `tasks`
expr_swap_t = list(results_data.iloc[0:,27]) # address by column number, not by name - name is too long
# (Note: see comment for line_swap_c eariler)

no_correct_expr_swap_t = 0
for i in range(0, len(expr_swap_t)):
  expr_swap_t[i] = expr_swap_t[i].replace(' ', '') # remove spaces
  expr_swap_t[i] = expr_swap_t[i].replace(';', '') # remove ";" (we assume to accept missing ";")
  if bool(re.search(correct_expr_swap_t, expr_swap_t[i], re.IGNORECASE)):
      no_correct_expr_swap_t = no_correct_expr_swap_t + 1

print("No. Correct Expressions (Swap/Task):", no_correct_expr_swap_t)
print("Correct Expressions (Swap/Task) (%):", no_correct_expr_swap_t / no_responses * 100)

# Find all values except outlier
ind_swap_t_noutl = results_data[results_data['Time spent swap task'] <
                                results_data['Time spent swap task'].mean() +
                                3 * results_data['Time spent swap task'].std()].index.tolist()

time_swap_t_avg = results_data['Time spent swap task'][ind_swap_t_noutl].mean()/60
time_swap_t_std = results_data['Time spent swap task'][ind_swap_t_noutl].std()/60
time_swap_t_min = results_data['Time spent swap task'][ind_swap_t_noutl].min()/60
time_swap_t_max = results_data['Time spent swap task'][ind_swap_t_noutl].max()/60

print("Avg Time Spent on `Swap/Task` (s):", time_swap_t_avg)
print("Std Time Spent on `Swap/Task` (s):", time_swap_t_std)
print("")

# Find information about finding bug in `Bubble sort' program with `checkpoints`

correct_line_bubble_c = 'Line 16' # correct line for `swap' program with `checkpoints`
line_bubble_c = results_data.iloc[0:,29] # address by column number, not by name - name is too long
# (Note: see comment for line_swap_c eariler)
no_correct_line_bubble_c = len([x for x in list(line_bubble_c) if correct_line_bubble_c in x]) # number of correct responses

print("No. Correct Responses (Bubble/Chechpoint):", no_correct_line_bubble_c)
print("Correct Responses (Bubble/Chechpoint) (%):", no_correct_line_bubble_c / no_responses * 100)

correct_expr_bubble_c = 'i\+\+' # correct expression for `swap' program with `checkpoints`
expr_bubble_c = list(results_data.iloc[0:,30]) # address by column number, not by name - name is too long

no_correct_expr_bubble_c = 0
for i in range(0, len(expr_bubble_c)):
  expr_bubble_c[i] = expr_bubble_c[i].replace(' ', '') # remove spaces
  expr_bubble_c[i] = expr_bubble_c[i].replace(';', '') # remove ";" (we assume to accept missing ";")
  if bool(re.search(correct_expr_bubble_c, expr_bubble_c[i])):
      no_correct_expr_bubble_c = no_correct_expr_bubble_c + 1

print("No. Correct Expressions (Bubble/Chechpoint):", no_correct_line_bubble_c)
print("Correct Expressions (Bubble/Chechpoint) (%):", no_correct_line_bubble_c / no_responses * 100)

# Find all values except outlier
ind_bubble_c_noutl = results_data[results_data['Time spent bubble sort checkpoint'] <
                                results_data['Time spent bubble sort checkpoint'].mean() +
                                3 * results_data['Time spent bubble sort checkpoint'].std()].index.tolist()

time_bubble_c_avg = results_data['Time spent bubble sort checkpoint'][ind_bubble_c_noutl].mean()/60
time_bubble_c_std = results_data['Time spent bubble sort checkpoint'][ind_bubble_c_noutl].std()/60
time_bubble_c_min = results_data['Time spent bubble sort checkpoint'][ind_bubble_c_noutl].min()/60
time_bubble_c_max = results_data['Time spent bubble sort checkpoint'][ind_bubble_c_noutl].max()/60

print("Avg Time Spent on `Bubble/Chechpoint` (s):", time_bubble_c_avg)
print("Std Time Spent on `Bubble/Chechpoint` (s):", time_bubble_c_std)
print("")

# Find information about finding bug in `Bubble sort' program with `tasks`

correct_line_bubble_t = 'Line 48' # correct line for `swap' program with `checkpoints`
line_bubble_t = results_data.iloc[0:,32] # address by column number, not by name - name is too long
no_correct_line_bubble_t = len([x for x in list(line_bubble_t) if correct_line_bubble_t in x]) # number of correct responses

print("No. Correct Responses (Bubble/Task):", no_correct_line_bubble_t)
print("Correct Responses (Bubble/Task) (%):", no_correct_line_bubble_t / no_responses * 100)

correct_expr_bubble_t = 'returntask_array_loop_incr' # correct expression for `swap' program with `checkpoints`
expr_bubble_t = list(results_data.iloc[0:,33]) # address by column number, not by name - name is too long

no_correct_expr_bubble_t = 0
for i in range(0, len(expr_bubble_t)):
  expr_bubble_t[i] = expr_bubble_t[i].replace(' ', '') # remove spaces
  expr_bubble_t[i] = expr_bubble_t[i].replace(';', '') # remove ";" (we assume to accept missing ";")
  if bool(re.search(correct_expr_bubble_t, expr_bubble_t[i])):
      no_correct_expr_bubble_t = no_correct_expr_bubble_t + 1

print("No. Correct Expressions (Bubble/Task):", no_correct_line_bubble_t)
print("Correct Expressions (Bubble/Task) (%):", no_correct_line_bubble_t / no_responses * 100)

# Find all values except outlier
ind_bubble_t_noutl = results_data[results_data['Time spent bubble sort task'] <
                                results_data['Time spent bubble sort task'].mean() +
                                3 * results_data['Time spent bubble sort task'].std()].index.tolist()

time_bubble_t_avg = results_data['Time spent bubble sort task'][ind_bubble_t_noutl].mean()/60
time_bubble_t_std = results_data['Time spent bubble sort task'][ind_bubble_t_noutl].std()/60
time_bubble_t_min = results_data['Time spent bubble sort task'][ind_bubble_t_noutl].min()/60
time_bubble_t_max = results_data['Time spent bubble sort task'][ind_bubble_t_noutl].max()/60

print("Avg Time Spent on `Bubble/Task` (s):", time_bubble_t_avg)
print("Std Time Spent on `Bubble/Task` (s):", time_bubble_t_std)
print("")

# Find information about finding bug in `Time' program with `checkpoints`

correct_line_time_c = 'Line 15' # correct line for `swap' program with `checkpoints`
line_time_c = results_data.iloc[0:,35] # address by column number, not by name - name is too long
no_correct_line_time_c = len([x for x in list(line_time_c) if correct_line_time_c in x]) # number of correct responses

print("No. Correct Responses (Time/Chechpoint):", no_correct_line_time_c)
print("Correct Responses (Time/Chechpoint) (%):", no_correct_line_time_c / no_responses * 100)

correct_expr_time_c1 = 'c\=a' # correct expression for `swap' program with `checkpoints`
correct_expr_time_c2 = 'c\=32.0f' # correct expression for `swap' program with `checkpoints`
correct_expr_time_c3 = 'c\=temperature_reading\*b\+a'
correct_expr_time_c = re.compile('|'.join([correct_expr_time_c1, correct_expr_time_c2, correct_expr_time_c3]))
expr_time_c = list(results_data.iloc[0:,36]) # address by column number, not by name - name is too long

no_correct_expr_time_c = 0
for i in range(0, len(expr_time_c)):
  expr_time_c[i] = expr_time_c[i].replace(' ', '') # remove spaces
  expr_time_c[i] = expr_time_c[i].replace(';', '') # remove ";" (we assume to accept missing ";")
  if bool(re.search(correct_expr_time_c, expr_time_c[i])):
      no_correct_expr_time_c = no_correct_expr_time_c + 1

print("No. Correct Expressions (Time/Chechpoint):", no_correct_line_time_c)
print("Correct Expressions (Time/Chechpoint) (%):", no_correct_line_time_c / no_responses * 100)

# Find all values except outlier
ind_time_c_noutl = results_data[results_data['Time spent time based checkpoint'] <
                                results_data['Time spent time based checkpoint'].mean() +
                                3 * results_data['Time spent time based checkpoint'].std()].index.tolist()

time_time_c_avg = results_data['Time spent time based checkpoint'][ind_time_c_noutl].mean()/60
time_time_c_std = results_data['Time spent time based checkpoint'][ind_time_c_noutl].std()/60
time_time_c_min = results_data['Time spent time based checkpoint'][ind_time_c_noutl].min()/60
time_time_c_max = results_data['Time spent time based checkpoint'][ind_time_c_noutl].max()/60

print("Avg Time Spent on `Time/Chechpoint` (s):", time_time_c_avg)
print("Std Time Spent on `Time/Chechpoint` (s):", time_time_c_std)
print("")

# Find information about finding bug in `Time' program with `tasks`

correct_line_time_t = 'Line 31' # correct line for `swap' program with `checkpoints`
line_time_t = results_data.iloc[0:,38] # address by column number, not by name - name is too long
no_correct_line_time_t = len([x for x in list(line_time_t) if correct_line_time_t in x]) # number of correct responses

print("No. Correct Responses (Time/Task):", no_correct_line_time_t)
print("Correct Responses (Time/Task) (%):", no_correct_line_time_t / no_responses * 100)

correct_expr_time_t1 = 'SET\(c,GET\(a\)\)' # correct expression for `swap' program with `checkpoints`
correct_expr_time_t2 = 'SET\(c,32.0f\)' # correct expression for `swap' program with `checkpoints`
correct_expr_time_t3 = 'SET\(c,GET\(temperature_reading\)\*GET\(b\)\+GET\(a\)\)' # correct expression for `swap' program with `checkpoints`
correct_expr_time_t = re.compile('|'.join([correct_expr_time_t1, correct_expr_time_t2, correct_expr_time_t3]))

expr_time_t = list(results_data.iloc[0:,39]) # address by column number, not by name - name is too long
# expr_time_t = list(results_data.iloc[0:,39]) # address by column number, not by name - name is too long
# (Note: see comment for line_swap_c eariler)

no_correct_expr_time_t = 0
for i in range(0, len(expr_time_t)):
  expr_time_t[i] = expr_time_t[i].replace(' ', '') # remove spaces
  expr_time_t[i] = expr_time_t[i].replace(';', '') # remove ";" (we assume to accept missing ";")
  #print(i, expr_time_t[i])
  if bool(re.search(correct_expr_time_t, expr_time_t[i])):
      no_correct_expr_time_t = no_correct_expr_time_t + 1
      #print("^")

print("No. Correct Expressions (Time/Chechpoint):", no_correct_line_time_t)
print("Correct Expressions (Time/Chechpoint) (%):", no_correct_line_time_t / no_responses * 100)

# Find all values except outlier
ind_time_t_noutl = results_data[results_data['Time spent time based task'] <
                                results_data['Time spent time based task'].mean() +
                                3 * results_data['Time spent time based task'].std()].index.tolist()

time_time_t_avg = results_data['Time spent time based task'][ind_time_t_noutl].mean()/60
time_time_t_std = results_data['Time spent time based task'][ind_time_t_noutl].std()/60
time_time_t_min = results_data['Time spent time based task'][ind_time_t_noutl].min()/60
time_time_t_max = results_data['Time spent time based task'][ind_time_t_noutl].max()/60

print("Avg Time Spent on `Time/Task` (s):", time_time_t_avg)
print("Std Time Spent on `Time/Task` (s):", time_time_t_std)
print("")

# Statistical analysis

wilcoxon_test_swap_app = stats.wilcoxon(results_data['Time spent swap task'],
                               results_data['Time spent swap checkpoint'])

wilcoxon_test_bubble_app = stats.wilcoxon(results_data['Time spent bubble sort task'],
                               results_data['Time spent bubble sort checkpoint'])

wilcoxon_test_time_app = stats.wilcoxon(results_data['Time spent time based task'],
                               results_data['Time spent time based checkpoint'])

print("Wilcoxon T Test (Swap):", wilcoxon_test_swap_app)
print("Wilcoxon T Test (Bubble):", wilcoxon_test_bubble_app)
print("Wilcoxon T Test (Time):", wilcoxon_test_time_app)
print("")

# Information about participants

# Self-assessment:

string_ee_edu = 'Formal electrical engineering education (university-level):How many years did you spend on any of these topics:'

null_ee_edu = len(results_data[results_data[string_ee_edu] == 'none'])
oney_ee_edu = len(results_data[results_data[string_ee_edu] == '1 year'])
twoy_ee_edu = len(results_data[results_data[string_ee_edu] == '2 years'])
threey_ee_edu = len(results_data[results_data[string_ee_edu] == '3 years'])
foury_ee_edu = len(results_data[results_data[string_ee_edu] == '4 years'])
fivey_ee_edu = len(results_data[results_data[string_ee_edu] == '5 years'])
fiveby_ee_edu = len(results_data[results_data[string_ee_edu] == 'more than 5 years'])

print("None (EE edu)", null_ee_edu / no_responses)
print("1 year (EE edu)", oney_ee_edu / no_responses)
print("2 years (EE edu)", twoy_ee_edu / no_responses)
print("3 years (EE edu)", threey_ee_edu / no_responses)
print("4 years (EE edu)", foury_ee_edu / no_responses)
print("5 years (EE edu)", fivey_ee_edu / no_responses)
print(">5 years (EE edu)", fiveby_ee_edu / no_responses)
print("")

# Self-assessment: "Embedded systems powered intermittently"

string_embed_inter_self = 'Embedded systems powered intermittently:Please self-assess yourself on the following question: ' \
                  '"Compared to others with similar background, age and education my knowledge of [XYZ] is", ' \
                  'where [XYZ] is'

sba_embed_inter_self = len(results_data[results_data[string_embed_inter_self] == 'Slightly below average'])
ba_embed_inter_self = len(results_data[results_data[string_embed_inter_self] == 'Below average'])
a_embed_inter_self = len(results_data[results_data[string_embed_inter_self] == 'Average'])
aa_embed_inter_self = len(results_data[results_data[string_embed_inter_self] == 'Slightly above average'])
saa_embed_inter_self = len(results_data[results_data[string_embed_inter_self] == 'Above average'])

print("Slightly below average (Embedded Intermittent):", sba_embed_inter_self / no_responses)
print("Below average (Embedded Intermittent):", ba_embed_inter_self / no_responses)
print("Average (Embedded Intermittent):", a_embed_inter_self / no_responses)
print("Slightly above average (Embedded Intermittent):", aa_embed_inter_self / no_responses)
print("Above average (Embedded Intermittent):", saa_embed_inter_self / no_responses)
print("")

# PL of choice
c_freq = len(results_data[results_data['My programming language of choice is'] == 'C'])
cpp_freq = len(results_data[results_data['My programming language of choice is'] == 'C++'])
java_freq = len(results_data[results_data['My programming language of choice is'] == 'Java'])
javasc_freq = len(results_data[results_data['My programming language of choice is'] == 'Java Script'])
python_freq = len(results_data[results_data['My programming language of choice is'] == 'Python'])
shell_freq = len(results_data[results_data['My programming language of choice is'] == 'Shell Script'])
scala_freq = len(results_data[results_data['My programming language of choice is'] == 'Scala'])
php_freq = len(results_data[results_data['My programming language of choice is'] == 'PHP'])
ruby_freq = len(results_data[results_data['My programming language of choice is'] == 'Ruby'])
rust_freq = len(results_data[results_data['My programming language of choice is'] == 'Rust'])
assembly_freq = len(results_data[results_data['My programming language of choice is'] == 'Assembly'])
other_freq = len(results_data[results_data['My programming language of choice is'] == 'Other'])

print("C:", c_freq / no_responses)
print("C++:", cpp_freq / no_responses)
print("Java:", java_freq / no_responses)
print("Java Script:", javasc_freq / no_responses)
print("Python:", python_freq / no_responses)
print("Shell Script:", shell_freq / no_responses)
print("Scala:", scala_freq / no_responses)
print("PHP:", php_freq / no_responses)
print("Ruby:", ruby_freq / no_responses)
print("Rust:", rust_freq / no_responses)
print("Assembly:", assembly_freq / no_responses)
print("Other:", other_freq / no_responses)
print("")

# Age
age_avg = results_data['My age is'].mean()
age_min = results_data['My age is'].min()
age_max = results_data['My age is'].max()

print("Age of participant (avg):", age_avg)
print("Age of participant (min):", age_min)
print("Age of participant (max):", age_max)
print("")

# Sex
woman_freq = len(results_data[results_data['I am a"'] == 'Woman'])
man_freq = len(results_data[results_data['I am a"'] == 'Man'])

print("Percentage of women:", woman_freq / no_responses)
print("Percentage of men:", man_freq / no_responses)
print("")

# Plot results

# Generic plot parameters
width = 0.15

# Plot response time
plt.figure(figsize = (4, 2))
ax = plt.subplot(111)

applications_c_avg_list = [time_swap_c_avg, time_bubble_c_avg, time_time_c_avg]
applications_c_std_list = [time_swap_c_std, time_bubble_c_std, time_time_c_std]

applications_t_avg_list = [time_swap_t_avg, time_bubble_t_avg, time_time_t_avg]
applications_t_std_list = [time_swap_t_std, time_bubble_t_std, time_time_t_std]

no_groups = len(applications_c_avg_list)
indices = np.arange(no_groups)

bar_c = ax.bar(indices, applications_c_avg_list, width, color = 'green',
               yerr = applications_c_std_list, ecolor = 'black',
               edgecolor = 'black', capsize = 2)
bar_t = ax.bar(indices + width, applications_t_avg_list, width, color='red',
               yerr = applications_t_std_list, ecolor = 'black',
               edgecolor='black', capsize = 2)

ax.set_ylim(0, max(applications_c_avg_list + applications_t_avg_list) +
            max(applications_c_std_list + applications_t_std_list) + 0.1)
ax.set_ylabel('Time to find a bug (min)')
xTickMarks = ['Swap', 'Bubble', 'Timekeeping']
ax.set_xticks(indices + width/2)
xtickNames = ax.set_xticklabels(xTickMarks)
plt.setp(xtickNames, fontsize=10)
ax.legend( (bar_c[0], bar_t[0]), ('TICS', 'InK'), loc = "upper left")
plt.show()

# Plot experience
plt.figure(figsize = (8, 2))
ax = plt.subplot(111)

xTickMarks = ['C', 'C++', 'Java', 'Java Script', 'Python', 'Shell', 'Scala', 'PHP', 'Ruby', 'Rust', 'Assembly', 'Other']
responses_experience = [c_freq, cpp_freq, java_freq, javasc_freq, python_freq, shell_freq, scala_freq, php_freq,
                        ruby_freq, rust_freq, assembly_freq, other_freq]

# Sort method taken from https://stackoverflow.com/questions/6618515/sorting-list-based-on-values-from-another-list
xTickMarks_sorted = [x for _,x in sorted(zip(responses_experience,xTickMarks), reverse=True)]
responses_experience_sorted = sorted(responses_experience, reverse=True)
responses_experience_sorted_nonzero_ind = np.nonzero(responses_experience_sorted)[0]

responses_experience_sorted_nonzero = responses_experience_sorted[0:responses_experience_sorted_nonzero_ind[-1]]
xTickMarks_sorted_nonzero = xTickMarks_sorted[0:responses_experience_sorted_nonzero_ind[-1]]

no_groups = len(responses_experience_sorted_nonzero)
indices = np.arange(no_groups)

bar_responses_experience = ax.bar(indices, responses_experience_sorted_nonzero,
                                  width, color = 'green')

ax.set_ylabel('No. responses')
ax.set_xticks(indices + width)
xtickNames = ax.set_xticklabels(xTickMarks_sorted_nonzero)
plt.setp(xtickNames, fontsize=10)

plt.show()

# Plot error rate of line

plt.figure(figsize = (8, 2))
ax = plt.subplot(111)

no_correct_line_c_list = [no_correct_line_swap_c, no_correct_line_bubble_c, no_correct_line_time_c]
no_correct_line_t_list = [no_correct_line_swap_t, no_correct_line_bubble_t, no_correct_line_time_t]

no_correct_line_c_list = [x / no_responses * 100 for x in no_correct_line_c_list]
no_correct_line_t_list = [x / no_responses * 100 for x in no_correct_line_t_list]

no_groups = len(no_correct_line_c_list)
indices = np.arange(no_groups)

bar_no_correct_line_c_list = ax.bar(indices, no_correct_line_c_list, width, color = 'green')
bar_no_correct_line_t_list = ax.bar(indices + width, no_correct_line_t_list, width, color='red')

ax.set_ylabel('Correctly identified line (%)')
xTickMarks = ['Swap', 'Bubble', 'Timekeeping']
ax.set_xticks(indices + width/2)
xtickNames = ax.set_xticklabels(xTickMarks)
plt.setp(xtickNames, fontsize=10)
ax.legend( (bar_no_correct_line_c_list[0], bar_no_correct_line_t_list[0]), ('TICS', 'InK'), loc = "lower center")

plt.show()

# Plot error rate responses

plt.figure(figsize = (8, 2))
ax = plt.subplot(111)

no_correct_expr_c_list = [no_correct_expr_swap_c, no_correct_expr_bubble_c, no_correct_expr_time_c]
no_correct_expr_t_list = [no_correct_expr_swap_t, no_correct_expr_bubble_t, no_correct_expr_time_t]

no_correct_expr_c_list = [x / no_responses * 100 for x in no_correct_expr_c_list]
no_correct_expr_t_list = [x / no_responses * 100 for x in no_correct_expr_t_list]

no_groups = len(no_correct_line_c_list)
indices = np.arange(no_groups)

bar_no_correct_expr_c_list = ax.bar(indices, no_correct_expr_c_list, width, color = 'green')
bar_no_correct_expr_t_list = ax.bar(indices + width, no_correct_expr_t_list, width, color='red')

ax.set_ylabel('Correct expressions (%)')
xTickMarks = ['Swap', 'Bubble', 'Timekeeping']
ax.set_xticks(indices + width/2)
xtickNames = ax.set_xticklabels(xTickMarks)
plt.setp(xtickNames, fontsize=10)
ax.legend( (bar_no_correct_expr_c_list[0], bar_no_correct_expr_t_list[0]), ('TICS', 'InK'), loc = "lower center")

plt.show()