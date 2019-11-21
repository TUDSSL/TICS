/* Subroutines for the gcc driver.
   Copyright (C) 2015-2017 Free Software Foundation, Inc.
   Contributed by Georg-Johann Lay <avr@gjlay.de>

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "diagnostic.h"
#include "tm.h"
#include "msp430-devices.h"

/* This structure is no longer being maintained.  Up-to-date data is read from
   a devices.csv file on the user's system.
   The data in this structure has been extracted from version 1.194 of the
   devices.csv file released by TI in September 2016.  */

struct t_msp430_mcu_data hard_msp430_mcu_data[] =
{
  { "cc430f5123",2,8 },
  { "cc430f5125",2,8 },
  { "cc430f5133",2,8 },
  { "cc430f5135",2,8 },
  { "cc430f5137",2,8 },
  { "cc430f5143",2,8 },
  { "cc430f5145",2,8 },
  { "cc430f5147",2,8 },
  { "cc430f6125",2,8 },
  { "cc430f6126",2,8 },
  { "cc430f6127",2,8 },
  { "cc430f6135",2,8 },
  { "cc430f6137",2,8 },
  { "cc430f6143",2,8 },
  { "cc430f6145",2,8 },
  { "cc430f6147",2,8 },
  { "msp430afe221",0,2 },
  { "msp430afe222",0,2 },
  { "msp430afe223",0,2 },
  { "msp430afe231",0,2 },
  { "msp430afe232",0,2 },
  { "msp430afe233",0,2 },
  { "msp430afe251",0,2 },
  { "msp430afe252",0,2 },
  { "msp430afe253",0,2 },
  { "msp430bt5190",2,8 },
  { "msp430c091",0,0 },
  { "msp430c092",0,0 },
  { "msp430c111",0,0 },
  { "msp430c1111",0,0 },
  { "msp430c112",0,0 },
  { "msp430c1121",0,0 },
  { "msp430c1331",0,0 },
  { "msp430c1351",0,0 },
  { "msp430c311s",0,0 },
  { "msp430c312",0,0 },
  { "msp430c313",0,0 },
  { "msp430c314",0,0 },
  { "msp430c315",0,0 },
  { "msp430c323",0,0 },
  { "msp430c325",0,0 },
  { "msp430c336",0,1 },
  { "msp430c337",0,1 },
  { "msp430c412",0,0 },
  { "msp430c413",0,0 },
  { "msp430cg4616",1,1 },
  { "msp430cg4617",1,1 },
  { "msp430cg4618",1,1 },
  { "msp430cg4619",1,1 },
  { "msp430e112",0,0 },
  { "msp430e313",0,0 },
  { "msp430e315",0,0 },
  { "msp430e325",0,0 },
  { "msp430e337",0,1 },
  { "msp430f110",0,0 },
  { "msp430f1101",0,0 },
  { "msp430f1101a",0,0 },
  { "msp430f1111",0,0 },
  { "msp430f1111a",0,0 },
  { "msp430f112",0,0 },
  { "msp430f1121",0,0 },
  { "msp430f1121a",0,0 },
  { "msp430f1122",0,0 },
  { "msp430f1132",0,0 },
  { "msp430f122",0,0 },
  { "msp430f1222",0,0 },
  { "msp430f123",0,0 },
  { "msp430f1232",0,0 },
  { "msp430f133",0,0 },
  { "msp430f135",0,0 },
  { "msp430f147",0,1 },
  { "msp430f1471",0,1 },
  { "msp430f148",0,1 },
  { "msp430f1481",0,1 },
  { "msp430f149",0,1 },
  { "msp430f1491",0,1 },
  { "msp430f155",0,0 },
  { "msp430f156",0,0 },
  { "msp430f157",0,0 },
  { "msp430f1610",0,1 },
  { "msp430f1611",0,1 },
  { "msp430f1612",0,1 },
  { "msp430f167",0,1 },
  { "msp430f168",0,1 },
  { "msp430f169",0,1 },
  { "msp430f2001",0,0 },
  { "msp430f2002",0,0 },
  { "msp430f2003",0,0 },
  { "msp430f2011",0,0 },
  { "msp430f2012",0,0 },
  { "msp430f2013",0,0 },
  { "msp430f2101",0,0 },
  { "msp430f2111",0,0 },
  { "msp430f2112",0,0 },
  { "msp430f2121",0,0 },
  { "msp430f2122",0,0 },
  { "msp430f2131",0,0 },
  { "msp430f2132",0,0 },
  { "msp430f2232",0,0 },
  { "msp430f2234",0,0 },
  { "msp430f2252",0,0 },
  { "msp430f2254",0,0 },
  { "msp430f2272",0,0 },
  { "msp430f2274",0,0 },
  { "msp430f233",0,2 },
  { "msp430f2330",0,2 },
  { "msp430f235",0,2 },
  { "msp430f2350",0,2 },
  { "msp430f2370",0,2 },
  { "msp430f2410",0,2 },
  { "msp430f2416",1,2 },
  { "msp430f2417",1,2 },
  { "msp430f2418",1,2 },
  { "msp430f2419",1,2 },
  { "msp430f247",0,2 },
  { "msp430f2471",0,2 },
  { "msp430f248",0,2 },
  { "msp430f2481",0,2 },
  { "msp430f249",0,2 },
  { "msp430f2491",0,2 },
  { "msp430f2616",1,2 },
  { "msp430f2617",1,2 },
  { "msp430f2618",1,2 },
  { "msp430f2619",1,2 },
  { "msp430f412",0,0 },
  { "msp430f413",0,0 },
  { "msp430f4132",0,0 },
  { "msp430f415",0,0 },
  { "msp430f4152",0,0 },
  { "msp430f417",0,0 },
  { "msp430f423",0,1 },
  { "msp430f423a",0,1 },
  { "msp430f425",0,1 },
  { "msp430f4250",0,0 },
  { "msp430f425a",0,1 },
  { "msp430f4260",0,0 },
  { "msp430f427",0,1 },
  { "msp430f4270",0,0 },
  { "msp430f427a",0,1 },
  { "msp430f435",0,0 },
  { "msp430f4351",0,0 },
  { "msp430f436",0,0 },
  { "msp430f4361",0,0 },
  { "msp430f437",0,0 },
  { "msp430f4371",0,0 },
  { "msp430f438",0,0 },
  { "msp430f439",0,0 },
  { "msp430f447",0,1 },
  { "msp430f448",0,1 },
  { "msp430f4481",0,1 },
  { "msp430f449",0,1 },
  { "msp430f4491",0,1 },
  { "msp430f4616",1,1 },
  { "msp430f46161",1,1 },
  { "msp430f4617",1,1 },
  { "msp430f46171",1,1 },
  { "msp430f4618",1,1 },
  { "msp430f46181",1,1 },
  { "msp430f4619",1,1 },
  { "msp430f46191",1,1 },
  { "msp430f47126",1,4 },
  { "msp430f47127",1,4 },
  { "msp430f47163",1,4 },
  { "msp430f47166",1,4 },
  { "msp430f47167",1,4 },
  { "msp430f47173",1,4 },
  { "msp430f47176",1,4 },
  { "msp430f47177",1,4 },
  { "msp430f47183",1,4 },
  { "msp430f47186",1,4 },
  { "msp430f47187",1,4 },
  { "msp430f47193",1,4 },
  { "msp430f47196",1,4 },
  { "msp430f47197",1,4 },
  { "msp430f477",0,0 },
  { "msp430f478",0,0 },
  { "msp430f4783",0,4 },
  { "msp430f4784",0,4 },
  { "msp430f479",0,0 },
  { "msp430f4793",0,4 },
  { "msp430f4794",0,4 },
  { "msp430f5131",2,8 },
  { "msp430f5132",2,8 },
  { "msp430f5151",2,8 },
  { "msp430f5152",2,8 },
  { "msp430f5171",2,8 },
  { "msp430f5172",2,8 },
  { "msp430f5212",2,8 },
  { "msp430f5213",2,8 },
  { "msp430f5214",2,8 },
  { "msp430f5217",2,8 },
  { "msp430f5218",2,8 },
  { "msp430f5219",2,8 },
  { "msp430f5222",2,8 },
  { "msp430f5223",2,8 },
  { "msp430f5224",2,8 },
  { "msp430f5227",2,8 },
  { "msp430f5228",2,8 },
  { "msp430f5229",2,8 },
  { "msp430f5232",2,8 },
  { "msp430f5234",2,8 },
  { "msp430f5237",2,8 },
  { "msp430f5239",2,8 },
  { "msp430f5242",2,8 },
  { "msp430f5244",2,8 },
  { "msp430f5247",2,8 },
  { "msp430f5249",2,8 },
  { "msp430f5252",2,8 },
  { "msp430f5253",2,8 },
  { "msp430f5254",2,8 },
  { "msp430f5255",2,8 },
  { "msp430f5256",2,8 },
  { "msp430f5257",2,8 },
  { "msp430f5258",2,8 },
  { "msp430f5259",2,8 },
  { "msp430f5304",2,8 },
  { "msp430f5308",2,8 },
  { "msp430f5309",2,8 },
  { "msp430f5310",2,8 },
  { "msp430f5324",2,8 },
  { "msp430f5325",2,8 },
  { "msp430f5326",2,8 },
  { "msp430f5327",2,8 },
  { "msp430f5328",2,8 },
  { "msp430f5329",2,8 },
  { "msp430f5333",2,8 },
  { "msp430f5335",2,8 },
  { "msp430f5336",2,8 },
  { "msp430f5338",2,8 },
  { "msp430f5340",2,8 },
  { "msp430f5341",2,8 },
  { "msp430f5342",2,8 },
  { "msp430f5358",2,8 },
  { "msp430f5359",2,8 },
  { "msp430f5418",2,8 },
  { "msp430f5418a",2,8 },
  { "msp430f5419",2,8 },
  { "msp430f5419a",2,8 },
  { "msp430f5435",2,8 },
  { "msp430f5435a",2,8 },
  { "msp430f5436",2,8 },
  { "msp430f5436a",2,8 },
  { "msp430f5437",2,8 },
  { "msp430f5437a",2,8 },
  { "msp430f5438",2,8 },
  { "msp430f5438a",2,8 },
  { "msp430f5500",2,8 },
  { "msp430f5501",2,8 },
  { "msp430f5502",2,8 },
  { "msp430f5503",2,8 },
  { "msp430f5504",2,8 },
  { "msp430f5505",2,8 },
  { "msp430f5506",2,8 },
  { "msp430f5507",2,8 },
  { "msp430f5508",2,8 },
  { "msp430f5509",2,8 },
  { "msp430f5510",2,8 },
  { "msp430f5513",2,8 },
  { "msp430f5514",2,8 },
  { "msp430f5515",2,8 },
  { "msp430f5517",2,8 },
  { "msp430f5519",2,8 },
  { "msp430f5521",2,8 },
  { "msp430f5522",2,8 },
  { "msp430f5524",2,8 },
  { "msp430f5525",2,8 },
  { "msp430f5526",2,8 },
  { "msp430f5527",2,8 },
  { "msp430f5528",2,8 },
  { "msp430f5529",2,8 },
  { "msp430f5630",2,8 },
  { "msp430f5631",2,8 },
  { "msp430f5632",2,8 },
  { "msp430f5633",2,8 },
  { "msp430f5634",2,8 },
  { "msp430f5635",2,8 },
  { "msp430f5636",2,8 },
  { "msp430f5637",2,8 },
  { "msp430f5638",2,8 },
  { "msp430f5658",2,8 },
  { "msp430f5659",2,8 },
  { "msp430f5xx_6xxgeneric",2,8 },
  { "msp430f6433",2,8 },
  { "msp430f6435",2,8 },
  { "msp430f6436",2,8 },
  { "msp430f6438",2,8 },
  { "msp430f6458",2,8 },
  { "msp430f6459",2,8 },
  { "msp430f6630",2,8 },
  { "msp430f6631",2,8 },
  { "msp430f6632",2,8 },
  { "msp430f6633",2,8 },
  { "msp430f6634",2,8 },
  { "msp430f6635",2,8 },
  { "msp430f6636",2,8 },
  { "msp430f6637",2,8 },
  { "msp430f6638",2,8 },
  { "msp430f6658",2,8 },
  { "msp430f6659",2,8 },
  { "msp430f6720",2,8 },
  { "msp430f6720a",2,8 },
  { "msp430f6721",2,8 },
  { "msp430f6721a",2,8 },
  { "msp430f6723",2,8 },
  { "msp430f6723a",2,8 },
  { "msp430f6724",2,8 },
  { "msp430f6724a",2,8 },
  { "msp430f6725",2,8 },
  { "msp430f6725a",2,8 },
  { "msp430f6726",2,8 },
  { "msp430f6726a",2,8 },
  { "msp430f6730",2,8 },
  { "msp430f6730a",2,8 },
  { "msp430f6731",2,8 },
  { "msp430f6731a",2,8 },
  { "msp430f6733",2,8 },
  { "msp430f6733a",2,8 },
  { "msp430f6734",2,8 },
  { "msp430f6734a",2,8 },
  { "msp430f6735",2,8 },
  { "msp430f6735a",2,8 },
  { "msp430f6736",2,8 },
  { "msp430f6736a",2,8 },
  { "msp430f6745",2,8 },
  { "msp430f67451",2,8 },
  { "msp430f67451a",2,8 },
  { "msp430f6745a",2,8 },
  { "msp430f6746",2,8 },
  { "msp430f67461",2,8 },
  { "msp430f67461a",2,8 },
  { "msp430f6746a",2,8 },
  { "msp430f6747",2,8 },
  { "msp430f67471",2,8 },
  { "msp430f67471a",2,8 },
  { "msp430f6747a",2,8 },
  { "msp430f6748",2,8 },
  { "msp430f67481",2,8 },
  { "msp430f67481a",2,8 },
  { "msp430f6748a",2,8 },
  { "msp430f6749",2,8 },
  { "msp430f67491",2,8 },
  { "msp430f67491a",2,8 },
  { "msp430f6749a",2,8 },
  { "msp430f67621",2,8 },
  { "msp430f67621a",2,8 },
  { "msp430f67641",2,8 },
  { "msp430f67641a",2,8 },
  { "msp430f6765",2,8 },
  { "msp430f67651",2,8 },
  { "msp430f67651a",2,8 },
  { "msp430f6765a",2,8 },
  { "msp430f6766",2,8 },
  { "msp430f67661",2,8 },
  { "msp430f67661a",2,8 },
  { "msp430f6766a",2,8 },
  { "msp430f6767",2,8 },
  { "msp430f67671",2,8 },
  { "msp430f67671a",2,8 },
  { "msp430f6767a",2,8 },
  { "msp430f6768",2,8 },
  { "msp430f67681",2,8 },
  { "msp430f67681a",2,8 },
  { "msp430f6768a",2,8 },
  { "msp430f6769",2,8 },
  { "msp430f67691",2,8 },
  { "msp430f67691a",2,8 },
  { "msp430f6769a",2,8 },
  { "msp430f6775",2,8 },
  { "msp430f67751",2,8 },
  { "msp430f67751a",2,8 },
  { "msp430f6775a",2,8 },
  { "msp430f6776",2,8 },
  { "msp430f67761",2,8 },
  { "msp430f67761a",2,8 },
  { "msp430f6776a",2,8 },
  { "msp430f6777",2,8 },
  { "msp430f67771",2,8 },
  { "msp430f67771a",2,8 },
  { "msp430f6777a",2,8 },
  { "msp430f6778",2,8 },
  { "msp430f67781",2,8 },
  { "msp430f67781a",2,8 },
  { "msp430f6778a",2,8 },
  { "msp430f6779",2,8 },
  { "msp430f67791",2,8 },
  { "msp430f67791a",2,8 },
  { "msp430f6779a",2,8 },
  { "msp430fe423",0,0 },
  { "msp430fe4232",0,0 },
  { "msp430fe423a",0,0 },
  { "msp430fe4242",0,0 },
  { "msp430fe425",0,0 },
  { "msp430fe4252",0,0 },
  { "msp430fe425a",0,0 },
  { "msp430fe427",0,0 },
  { "msp430fe4272",0,0 },
  { "msp430fe427a",0,0 },
  { "msp430fg4250",0,0 },
  { "msp430fg4260",0,0 },
  { "msp430fg4270",0,0 },
  { "msp430fg437",0,0 },
  { "msp430fg438",0,0 },
  { "msp430fg439",0,0 },
  { "msp430fg4616",1,1 },
  { "msp430fg4617",1,1 },
  { "msp430fg4618",1,1 },
  { "msp430fg4619",1,1 },
  { "msp430fg477",0,0 },
  { "msp430fg478",0,0 },
  { "msp430fg479",0,0 },
  { "msp430fg6425",2,8 },
  { "msp430fg6426",2,8 },
  { "msp430fg6625",2,8 },
  { "msp430fg6626",2,8 },
  { "msp430fr2032",2,0 },
  { "msp430fr2033",2,0 },
  { "msp430fr2110",2,0 },
  { "msp430fr2111",2,0 },
  { "msp430fr2310",2,0 },
  { "msp430fr2311",2,0 },
  { "msp430fr2433",2,8 },
  { "msp430fr2532",2,8 },
  { "msp430fr2533",2,8 },
  { "msp430fr2632",2,8 },
  { "msp430fr2633",2,8 },
  { "msp430fr2xx_4xxgeneric",2,8 },
  { "msp430fr4131",2,0 },
  { "msp430fr4132",2,0 },
  { "msp430fr4133",2,0 },
  { "msp430fr5720",2,8 },
  { "msp430fr5721",2,8 },
  { "msp430fr5722",2,8 },
  { "msp430fr5723",2,8 },
  { "msp430fr5724",2,8 },
  { "msp430fr5725",2,8 },
  { "msp430fr5726",2,8 },
  { "msp430fr5727",2,8 },
  { "msp430fr5728",2,8 },
  { "msp430fr5729",2,8 },
  { "msp430fr5730",2,8 },
  { "msp430fr5731",2,8 },
  { "msp430fr5732",2,8 },
  { "msp430fr5733",2,8 },
  { "msp430fr5734",2,8 },
  { "msp430fr5735",2,8 },
  { "msp430fr5736",2,8 },
  { "msp430fr5737",2,8 },
  { "msp430fr5738",2,8 },
  { "msp430fr5739",2,8 },
  { "msp430fr57xxgeneric",2,8 },
  { "msp430fr5847",2,8 },
  { "msp430fr58471",2,8 },
  { "msp430fr5848",2,8 },
  { "msp430fr5849",2,8 },
  { "msp430fr5857",2,8 },
  { "msp430fr5858",2,8 },
  { "msp430fr5859",2,8 },
  { "msp430fr5867",2,8 },
  { "msp430fr58671",2,8 },
  { "msp430fr5868",2,8 },
  { "msp430fr5869",2,8 },
  { "msp430fr5870",2,8 },
  { "msp430fr5872",2,8 },
  { "msp430fr58721",2,8 },
  { "msp430fr5887",2,8 },
  { "msp430fr5888",2,8 },
  { "msp430fr5889",2,8 },
  { "msp430fr58891",2,8 },
  { "msp430fr5922",2,8 },
  { "msp430fr59221",2,8 },
  { "msp430fr5947",2,8 },
  { "msp430fr59471",2,8 },
  { "msp430fr5948",2,8 },
  { "msp430fr5949",2,8 },
  { "msp430fr5957",2,8 },
  { "msp430fr5958",2,8 },
  { "msp430fr5959",2,8 },
  { "msp430fr5962",2,8 },
  { "msp430fr5964",2,8 },
  { "msp430fr5967",2,8 },
  { "msp430fr5968",2,8 },
  { "msp430fr5969",2,8 },
  { "msp430fr59691",2,8 },
  { "msp430fr5970",2,8 },
  { "msp430fr5972",2,8 },
  { "msp430fr59721",2,8 },
  { "msp430fr5986",2,8 },
  { "msp430fr5987",2,8 },
  { "msp430fr5988",2,8 },
  { "msp430fr5989",2,8 },
  { "msp430fr59891",2,8 },
  { "msp430fr5992",2,8 },
  { "msp430fr5994",2,8 },
  { "msp430fr59941",2,8 },
  { "msp430fr5xx_6xxgeneric",2,8 },
  { "msp430fr6820",2,8 },
  { "msp430fr6822",2,8 },
  { "msp430fr68221",2,8 },
  { "msp430fr6870",2,8 },
  { "msp430fr6872",2,8 },
  { "msp430fr68721",2,8 },
  { "msp430fr6877",2,8 },
  { "msp430fr6879",2,8 },
  { "msp430fr68791",2,8 },
  { "msp430fr6887",2,8 },
  { "msp430fr6888",2,8 },
  { "msp430fr6889",2,8 },
  { "msp430fr68891",2,8 },
  { "msp430fr6920",2,8 },
  { "msp430fr6922",2,8 },
  { "msp430fr69221",2,8 },
  { "msp430fr6927",2,8 },
  { "msp430fr69271",2,8 },
  { "msp430fr6928",2,8 },
  { "msp430fr6970",2,8 },
  { "msp430fr6972",2,8 },
  { "msp430fr69721",2,8 },
  { "msp430fr6977",2,8 },
  { "msp430fr6979",2,8 },
  { "msp430fr69791",2,8 },
  { "msp430fr6987",2,8 },
  { "msp430fr6988",2,8 },
  { "msp430fr6989",2,8 },
  { "msp430fr69891",2,8 },
  { "msp430fw423",0,0 },
  { "msp430fw425",0,0 },
  { "msp430fw427",0,0 },
  { "msp430fw428",0,0 },
  { "msp430fw429",0,0 },
  { "msp430g2001",0,0 },
  { "msp430g2101",0,0 },
  { "msp430g2102",0,0 },
  { "msp430g2111",0,0 },
  { "msp430g2112",0,0 },
  { "msp430g2113",0,0 },
  { "msp430g2121",0,0 },
  { "msp430g2131",0,0 },
  { "msp430g2132",0,0 },
  { "msp430g2152",0,0 },
  { "msp430g2153",0,0 },
  { "msp430g2201",0,0 },
  { "msp430g2202",0,0 },
  { "msp430g2203",0,0 },
  { "msp430g2210",0,0 },
  { "msp430g2211",0,0 },
  { "msp430g2212",0,0 },
  { "msp430g2213",0,0 },
  { "msp430g2221",0,0 },
  { "msp430g2230",0,0 },
  { "msp430g2231",0,0 },
  { "msp430g2232",0,0 },
  { "msp430g2233",0,0 },
  { "msp430g2252",0,0 },
  { "msp430g2253",0,0 },
  { "msp430g2302",0,0 },
  { "msp430g2303",0,0 },
  { "msp430g2312",0,0 },
  { "msp430g2313",0,0 },
  { "msp430g2332",0,0 },
  { "msp430g2333",0,0 },
  { "msp430g2352",0,0 },
  { "msp430g2353",0,0 },
  { "msp430g2402",0,0 },
  { "msp430g2403",0,0 },
  { "msp430g2412",0,0 },
  { "msp430g2413",0,0 },
  { "msp430g2432",0,0 },
  { "msp430g2433",0,0 },
  { "msp430g2444",0,0 },
  { "msp430g2452",0,0 },
  { "msp430g2453",0,0 },
  { "msp430g2513",0,0 },
  { "msp430g2533",0,0 },
  { "msp430g2544",0,0 },
  { "msp430g2553",0,0 },
  { "msp430g2744",0,0 },
  { "msp430g2755",0,0 },
  { "msp430g2855",0,0 },
  { "msp430g2955",0,0 },
  { "msp430i2020",0,2 },
  { "msp430i2021",0,2 },
  { "msp430i2030",0,2 },
  { "msp430i2031",0,2 },
  { "msp430i2040",0,2 },
  { "msp430i2041",0,2 },
  { "msp430i2xxgeneric",0,2 },
  { "msp430l092",0,0 },
  { "msp430p112",0,0 },
  { "msp430p313",0,0 },
  { "msp430p315",0,0 },
  { "msp430p315s",0,0 },
  { "msp430p325",0,0 },
  { "msp430p337",0,1 },
  { "msp430sl5438a",2,8 },
  { "msp430tch5e",0,0 },
  { "msp430xgeneric",2,8 },
  { "rf430f5144",2,8 },
  { "rf430f5155",2,8 },
  { "rf430f5175",2,8 },
  { "rf430frl152h",0,0 },
  { "rf430frl152h_rom",0,0 },
  { "rf430frl153h",0,0 },
  { "rf430frl153h_rom",0,0 },
  { "rf430frl154h",0,0 },
  { "rf430frl154h_rom",0,0 }
};

char * driver_devices_csv_loc = NULL;

static int
parse_devices_csv_wrapper (const char * mcu_name)
{
  if (driver_devices_csv_loc)
    parse_devices_csv (driver_devices_csv_loc, mcu_name);
  else if (devices_csv_loc)
    parse_devices_csv (devices_csv_loc, mcu_name);
  else
    return 1;
  return 0;
}

const char *
msp430_check_path_for_devices (int argc, const char **argv)
{
  /* devices_csv_loc is set by the option.  */
  if (devices_csv_loc)
    return NULL;
  else if (driver_devices_csv_loc)
    return concat ("-mdevices-csv-loc=", driver_devices_csv_loc, NULL);
  const char dirsep[2] = { DIR_SEPARATOR, 0 };
  FILE * devices_csv = NULL;
  char * local_devices_csv_loc = NULL;
  int i;
  for (i = 0; i < argc; i++)
    {
      char *inc_path = ASTRDUP (argv[i]);
      if (!IS_DIR_SEPARATOR (inc_path[strlen (inc_path) - 1]))
	inc_path = concat (inc_path, dirsep, NULL);
      local_devices_csv_loc = concat (inc_path, "devices.csv", NULL);
      devices_csv = fopen (local_devices_csv_loc, "r");
      if (devices_csv != NULL)
	{
	  fclose (devices_csv);
	  driver_devices_csv_loc = local_devices_csv_loc;
	  return concat ("-mdevices-csv-loc=", local_devices_csv_loc, NULL);
	}
    }
  return NULL;
}

const char *
msp430_select_cpu (int argc, const char ** argv)
{
  if (argc == 0 || parse_devices_csv_wrapper (argv[0]))
    return NULL;
  switch (extracted_mcu_data.revision)
    {
    case 0:
      return "-mcpu=msp430";
    case 1:
      return "-mcpu=msp430x";
    case 2:
      return "-mcpu=msp430xv2";
    default:
      error ("Unexpected error get msp430 cpu type\n");
      return NULL;
    }
}

/* Implement spec function `msp430_hwmult_lib´.  */

const char *
msp430_select_hwmult_lib (int argc ATTRIBUTE_UNUSED, const char ** argv ATTRIBUTE_UNUSED)
{
  int i;
  switch (argc)
  {
  case 1:
    if (strcasecmp (argv[0], "default"))
      error ("unexpected argument to msp430_select_hwmult_lib: %s", argv[0]);
    break;

  default:
    /* We can get three or more arguments passed to this function.
       This happens when the same option is repeated on the command line.
       For example:
         msp430-elf-gcc -mhwmult=none -mhwmult=16bit foo.c
       We have to use the last argument as our selector.  */
    if (strcasecmp (argv[0], "hwmult") == 0)
      {
	static struct hwmult_options
	{
	  const char * name;
	  const char * lib;
	} hwmult_options [] =
	{
	  { "none", "-lmul_none" },
	  { "auto", "-lmul_AUTO" }, /* Should not see this one... */
	  { "16bit", "-lmul_16" },
	  { "32bit", "-lmul_32" },
	  { "f5series", "-lmul_f5" }
	};

	for (i = ARRAY_SIZE (hwmult_options); i--;)
	  if (strcasecmp (argv[argc - 1], hwmult_options[i].name) == 0)
	    return hwmult_options[i].lib;
      }
    else if (strcasecmp (argv[0], "mcu") == 0)
      {
	parse_devices_csv_wrapper (argv[argc - 1]);
	if (extracted_mcu_data.name == NULL)
	  {
	    for (i = ARRAY_SIZE (hard_msp430_mcu_data); i--;)
	      if (strcasecmp (argv[argc - 1],
			      hard_msp430_mcu_data[i].name) == 0)
		extracted_mcu_data = hard_msp430_mcu_data[i];
	  }
	switch (extracted_mcu_data.hwmpy)
	  {
	  case 0: return "-lmul_none";
	  case 2:
	  case 1: return "-lmul_16";
	  case 4: return "-lmul_32";
	  case 8: return "-lmul_f5";
	  default:
		  error ("unrecognized hwpy field in hard_msp430_mcu_data[%d]: %d",
			 i, hard_msp430_mcu_data[i].hwmpy);
		  break;
	  }
      }
    else
      error ("unexpected first argument to msp430_select_hwmult_lib: %s", argv[0]);
    break;

  case 0:
    error ("msp430_select_hwmult_lib needs one or more arguments");
    break;
  }
  
  return "-lmul_none";
}

const char *
msp430_get_linker_devices_include_path (int argc ATTRIBUTE_UNUSED, const char **argv ATTRIBUTE_UNUSED)
{
  char *devices_csv_path;
  if (check_env_var_for_devices (&devices_csv_path))
    return NULL;
  return concat ("-L", msp430_dirname (devices_csv_path), NULL);
}
