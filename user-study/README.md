# TICS Programming Languaage User Study Files

This sub-repository contains files used in the analysis of the user study availabe at [http://sgiz.mobi/s3/New-programming-language-user-study](http://sgiz.mobi/s3/New-programming-language-user-study) (last check of link validity: December 4, 2019). The results of this study have been presented in the following paper

```

@inproceedings{kortbeek_asplos2020,
    author = "Vito {Kortbeek} and Kasim Sinan {Yildirim} and Abu {Bakar} and Jacob {Sorber}
    and Josiah {Hester} and Przemys{\l}}aw {Pawe{\l}czak}",
    title = "Time-sensitive Intermittent Computing Meets Legacy Software",
    year = "2020",
    booktitle = "Proc. ASPLOS",
    address = "Lausanne, Switzerland",
    month = mar  # " 16--20,",
    publisher = "ACM"
}

```

### Structure of this directory

- `user-study-analysis`: Python files for data analysis, i.e.
  - `anonymize-user-study.py`: file for anonymization of survery data 
  - `analyze-user-study.py`: file for data analysis and plotting
- `user-study-questions`: Exact list of questions posed to the user study participants, i.e. 
  - `user_study_questions_surveygizmo_downloaded_24_april_2019.doc`: survey questions in MS Word format
  - `user_study_questions_surveygizmo_downloaded_24_april_2019.pdf`: survey questions in PDF format
- `user-study-results`: CSV files containg user study results, i.e. 
  - `mturk`: directory containing `Batch_3492166_batch_results_anonymized.csv` file, i.e. file containging proof of work of Amazon Mechanical Turk participants
  - `surveygizmo`: directory containing `20190109034400-SurveyExport_anonymized.csv` file, i.e. file containg survey results (both Amazon Mechanical Turk and non-Amazon Mechanical Turk participants)

### History

- _December 4, 2019_: The first complete response of the user study has been submited on December 21, 2018 and the last valid resopnse has been submitted on May 30, 2019 (all submitted using [www.surveygizmo.com](https://www.surveygizmo.com) platform. Some inputs have been crowdsourced using [Amazon Mechanical Turk](https://www.mturk.com/) platform: the first input was submitted on January 12, 2019 and the last input was submitted on January 17, 2019.

### Copyright

Copyright (C) 2019 TU Delft Embedded and Networked Systems Group/Sustainable Systems Laboratory.

MIT Licence. See [License](https://github.com/TUDSSL/TICS/blob/master/LICENSE) file for details.
