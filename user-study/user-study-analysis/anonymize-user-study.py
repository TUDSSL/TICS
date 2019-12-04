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

import pandas as pd
from pathlib import Path

# Anonymize Surveygizmo results

results_folder = Path("../user-study-results/surveygizmo")
results_file = results_folder / "20190109034400-SurveyExport_anonymized.csv" # Load unanonymized file
results_data = pd.read_csv(results_file, keep_default_na = False, dtype = object)

try:
    results_data['Extended Referer']
except:
    pass
else:
    results_data.loc[results_data['Extended Referer'] != None, 'Extended Referer'] = 'Anonymized'
    results_data.loc[results_data['Extended User Agent'] != None, 'Extended User Agent'] = 'Anonymized'

results_data.loc[results_data['Referer'] != None, 'Referer'] = 'Anonymized'
results_data.loc[results_data['SessionID'] != None, 'SessionID'] = 'Anonymized'
results_data.loc[results_data['User Agent'] != None, 'User Agent'] = 'Anonymized'
results_data.loc[results_data['IP Address'] != None, 'IP Address'] = 'Anonymized'
results_data.loc[results_data['Longitude'] != None, "Longitude"] = 'Anonymized'
results_data.loc[results_data['Latitude'] != None, 'Latitude'] = 'Anonymized'
results_data.loc[results_data['Country'] != None, 'Country'] = 'Anonymized'
results_data.loc[results_data['City'] != None, 'City'] = 'Anonymized'
results_data.loc[results_data['State/Region'] != None, 'State/Region'] = 'Anonymized'
results_data.loc[results_data['Postal'] != None, 'Postal'] = 'Anonymized'

results_data.to_csv(results_file, index=False)


# Anonymize connected Amazon Mechanical Turk results

results_folder = Path("../user-study-results/mturk")
results_file = results_folder / "Batch_3492166_batch_results_anonymized.csv" # Load unanonymized file
results_data = pd.read_csv(results_file, keep_default_na = False, dtype = object)

results_data.loc[results_data['HITId'] != None, 'HITId'] = 'Anonymized'
results_data.loc[results_data['HITTypeId'] != None, 'HITTypeId'] = 'Anonymized'
results_data.loc[results_data['RequesterAnnotation'] != None, 'RequesterAnnotation'] = 'Anonymized'
results_data.loc[results_data['AssignmentId'] != None, 'AssignmentId'] = 'Anonymized'
results_data.loc[results_data['WorkerId'] != None, 'WorkerId'] = 'Anonymized'

results_data.to_csv(results_file, index=False)