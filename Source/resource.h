/*
 * Copyright (c) 2022, Benichou Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purpose : Ressource file. Used by ITN Converter.rc
 */

#define STR_(x) #x
#define STR(x) STR_(x)

#define SOFT_VER_MAJOR  1
#define SOFT_VER_MINOR  9
#define SOFT_VER_PATCH  6
#define SOFT_VER_BUILD  0

#define SOFT_NAME       "ITN Converter"
#define INTERNAL_NAME   "ITNConv"
#define REGISTRY_KEY    "Software\\ITNConv"
#define SOFT_COPYRIGHT  "Copyright (C) 2004 - 2022"
#define SOFT_COMPAGNY   "Benichou Software"
#define SOFT_URL        "http://www.benichou-software.com"
#define BASE_URL        "http://itnconv.benitools.info/planner/188/"
#define GGM_API_KEY     "YOUR API KEY" // Google API key
#define MBM_API_KEY     "YOUR API KEY" // Microsoft Bing API key
#define VMM_API_KEY     "YOUR API KEY" // ViaMichemin API key
#define GNS_API_KEY     "YOUR API KEY" // GeoNames API key
#define HERE_API_ID     "YOUR API KEY" // Here API ID
#define HERE_API_KEY    "YOUR API KEY" // Here API key
#define GEOPORTAL_API_KEY "YOUR API KEY" // GeoPortal API key
#define GEOPORTAL_API_REFERER BASE_URL
#define TOMTOM_API_KEY  "YOUR API KEY" // TOMTOM API key
#define TOMTOM_ROUTING_API_KEY "YOUR API KEY" // TOMTOM Routing API key
#define TOMTOM_SEARCH_API_KEY "YOUR API KEY" // TOMTOM Search API key

#ifdef _MOBILE_
#define SOFT_VER_MOBILE "M"
#else
#define SOFT_VER_MOBILE ""
#endif

#if SOFT_VER_BUILD > 0
#define SOFT_VER_RC " RC" STR(SOFT_VER_BUILD)
#else
#define SOFT_VER_RC ""
#endif

#define SOFT_VERSION    STR(SOFT_VER_MAJOR) "." STR(SOFT_VER_MINOR) STR(SOFT_VER_PATCH) SOFT_VER_MOBILE SOFT_VER_RC
#define SOFT_FULL_NAME  SOFT_NAME " " SOFT_VERSION

 // Custom
#define IDR_MAINFRAME                   128
#define IDR_AXESRC                      129
#define IDR_PTMSRC                      130
#define IDR_ESTSRC                      131

// Control
#define IDOPEN                          1000
#define IDEXPORT                        1001
#define IDC_BUTTON_SEARCH               1002
#define IDC_LIST                        1003
#define IDC_FILENAME                    1004
#define IDC_RADIO_ITN                   1005
#define IDC_POINT_NUMBER                1005
#define IDC_RADIO_GPX                   1006
#define IDC_NAME                        1006
#define IDC_RADIO_RTE                   1007
#define IDC_DISTANCE                    1007
#define IDC_CHECK_REMOVE_COMMA          1008
#define IDC_CHECK_OPTION                1008
#define IDC_COMBO_FILEEXPORT            1009
#define IDC_SOFT_VERSION                1010
#define IDC_LABEL_EXPORT                1011
#define IDC_COMBO_CUT_ITN               1012
#define IDC_EDIT_CUT_ITN                1013
#define IDC_STATIC_OPTIONS              1014
#define IDC_WEBLINK                     1015
#define IDC_BUTTON_ADD                  1018
#define IDC_BUTTON_REMOVE               1019
#define IDC_BUTTON_UP                   1020
#define IDC_BUTTON_DOWN                 1021
#define IDC_STATIC_UP                   1022
#define IDC_STATIC_DOWN                 1023
#define IDC_STATIC_ADD                  1024
#define IDC_EDIT_LATITUDE               1024
#define IDC_STATIC_REMOVE               1025
#define IDC_EDIT_LONGITUDE              1025
#define IDC_EDIT_ADDRESS                1026
#define IDC_STATIC_LATITUDE             1027
#define IDC_BUTTON_INVERT               1027
#define IDC_STATIC_LONGITUDE            1028
#define IDC_STATIC_INVERT               1028
#define IDC_BUTTON_VIEWALL              1028
#define IDC_STATIC_ADDRESS              1029
#define IDC_BUTTON_CALCULATE            1029
#define IDC_EDIT_NAME                   1030
#define IDC_STATIC_TEMPLATE             1031
#define IDC_EDIT_SEARCH                 1031
#define IDC_STATIC_SEARCH               1032
#define IDC_BUTTON_EDITOR               1032
#define IDC_LIST_SEARCH                 1033
#define IDC_STATIC_EDITOR               1033
#define IDC_STATIC_RESULTS              1034
#define IDC_EXPLORER                    1035
#define IDC_STATIC_STATUS               1036
#define IDC_BUTTON_DELROUTE             1037
#define IDC_BUTTON_REMOVEALL            1038
#define IDC_PROGRESS                    1039
#define IDC_BUTTON_GOBACK               1040
#define IDC_BUTTON_OPTIMISE             1041
#define IDC_POI_TREE                    1041
#define IDC_BUTTON_POI                  1042
#define IDC_STATIC_NOTE                 1042
#define IDC_IMAGE                       1043
#define IDC_LIST_PROXIMITY              1043
#define IDC_COMBO_ZOOM                  1044
#define IDC_STATIC_PROXIMITY            1044
#define IDC_TAB_SEARCH                  1045
#define IDC_BUTTON_CONFIG               1045
#define IDC_LIST_FAVORITES              1046
#define IDC_SEARCH_LOCATION             1048
#define IDC_SEARCH_PROXIMITY            1049
#define IDC_CHECK_AUTO_CALC             1050
#define IDC_DISPLAY_FAVORITE            1050
#define IDC_CHECK_MODE_RANDO            1051
#define IDC_CHECK_DIST_MILES            1052
#define IDC_CHECK_FORCE_ROAD            1053
#define IDC_COMBO_COORD_FORMAT          1054
#define IDC_STATIC_COORD_FORMAT         1055
#define IDC_STATIC_BACKGROUND           1056
#define IDC_EDIT_SNIPPET                1057
#define IDC_CHECK_AVOID_HIGHWAY         1057
#define IDC_STATIC_SNIPPET              1058
#define IDC_CHECK_AVOID_TOLLS           1058
#define IDC_EDIT_ALTITUDE               1059
#define IDC_STATIC_SDECIMAL             1059
#define IDC_STATIC_ALTITUDE             1060
#define IDC_COMBO_LATITUDE              1061
#define IDC_COMBO_LONGITUDE             1062
#define IDC_COMBO_ALTITUDE              1063
#define IDC_COMBO_ADDRESS               1064
#define IDC_COMBO_SNIPPET               1065
#define IDC_COMBO_TEMPLATE              1066
#define IDC_CHECK_LOCALE                1067
#define IDC_EDIT_SDECIMAL               1068
#define IDC_STATIC_SLIST                1069
#define IDC_EDIT_SLIST                  1070
#define IDC_STATIC_OPT_ITN              1070
#define IDC_STATIC_ROUTE_PROVIDER       1071
#define IDC_COMBO_ROUTE_PROVIDER        1072
#define IDC_CHECK_APPEND                1073
#define IDC_TREE_MAPS                   1074
#define IDC_CHECK_AUTO_ADD_STEP         1075
#define IDC_TREE_CHOOSE_ARRAY           1076
#define IDC_BUTTON_SELECT_ALL           1077
#define IDC_BUTTON_UNSELECT_ALL         1078
#define IDC_BUTTON_PRINT                1079
#define IDC_STATIC_ITINERARY_TYPE       1080
#define IDC_COMBO_ITINERARY_TYPE        1081
#define IDC_COMBO_LAT_NS                1082
#define IDC_EDIT_LAT_DEG                1083
#define IDC_EDIT_LAT_DMS_MIN            1084
#define IDC_EDIT_LAT_DM_MIN             1085
#define IDC_EDIT_LAT_SEC                1086
#define IDC_COMBO_LNG_WE                1087
#define IDC_EDIT_LNG_DEG                1088
#define IDC_EDIT_LNG_DMS_MIN            1089
#define IDC_EDIT_LNG_DM_MIN             1090
#define IDC_EDIT_LNG_SEC                1091
#define IDC_STATIC_LAT_DEG              1092
#define IDC_STATIC_LAT_MIN              1093
#define IDC_STATIC_LAT_SEC              1094
#define IDC_STATIC_LNG_DEG              1095
#define IDC_STATIC_LNG_MIN              1096
#define IDC_STATIC_LNG_SEC              1097
#define IDC_STATIC_COORDS_FORMAT        1098
#define IDC_COMBO_COORDS_FORMAT         1099
#define IDC_STATIC_OPT_DISPLAY          1100
#define IDC_CHECK_DISPLAY_NUMBER        1101
#define IDC_STATIC_VEHICLE_TYPE         1102
#define IDC_COMBO_VEHICLE_TYPE          1103

// IDD_PROGRESS_DLG
#define IDC_PROGRESS_BAR                1001
#define IDC_PROGRESS_MESSAGE            1002
#define IDC_PROGRESS_ICON               1003

// Menu
#define IDM_ABOUTBOX                    0x0010

// Dialog
#define IDD_ABOUTBOX                    100
#define IDD_ITNCONVERTER_DIALOG         101
#define IDD_APPEND_FRAME                102
#define IDD_INSERT_MODIFY               103
#define IDD_DIALOG_EDITOR               104
#define IDD_POI_MANAGER                 105
#define IDD_DIALOG_CONFIG               106
#define IDD_DIALOG_CSV                  107
#define IDD_PROGRESS_DLG                108
#define IDD_DIALOG_CHOOSE_ARRAY         109

// Cursor
#define IDC_MOVE                        100

// Bitmap
#define IDB_ADD                         138
#define IDB_REMOVE                      139
#define IDB_UP                          140
#define IDB_DOWN                        141
#define IDB_FREE_142                    142
#define IDB_INVERT                      145
#define IDB_ITI_MOTORBIKE               147
#define IDB_ITI_LINES                   148
#define IDB_ITI_CAR                     149
#define IDB_EARTH                       150
#define IDB_COMPASS                     154
#define IDB_SEARCH                      155
#define IDB_CLEAR_CAR                   158
#define IDB_REMOVEALL                   159
#define IDB_OPTIMISE                    160
#define IDB_POI                         161
#define IDB_LOCATION                    162
#define IDB_PROXIMITY                   163
#define IDB_ITI_RANDO                   167
#define IDB_CONFIG                      168
#define IDB_FAVORITE                    170
#define IDB_NVG_FRESH                   172
#define IDB_REMOVE_POPUP                173
#define IDB_MODIFY_POPUP                174
#define IDB_OPEN_POPUP                  175
#define IDB_SAVE_POPUP                  176
#define IDB_VIRTUAL_EARTH               178
#define IDB_GOOGLE_MAP                  179
#define IDB_TOMTOM_HOME                 180
#define IDB_TOMTOM_MAP                  181
#define IDB_SEARCH_SEL                  182
#define IDB_SEARCH_DISABLED             183
#define IDB_PIONEER_NAVGATE             184
#define IDB_MIOMORE_DESKTOP             185
#define IDB_GARMIN_COMMUNICATOR         186
#define IDB_CHOOSE_ICON                 187
#define IDB_PRINT                       188
#define IDB_ITI_BIKE                    189
#define IDB_ITI_TRUCK                   190
#define IDB_191                         191
#define IDB_BING                        192
#define IDB_GOOGLE                      193
#define IDB_VIAMICHELIN                 194
#define IDB_OSM                         195
#define IDB_WAZE                        196
#define IDB_GOBACK                      197
#define IDB_SELECT_ALL                  198
#define IDB_UNSELECT_ALL                199
#define IDB_MAPS_ICON                   200
#define IDB_HERE                        201
#define IDB_TOMTOM                      202
#define IDB_HERE_MAP                    203
#define IDB_GEOPORTAL                   204

// String Table
#define IDS_ABOUTBOX                    101
#define IDS_LONGITUDE                   102
#define IDS_LATITUDE                    103
#define IDS_ADDRESS                     104
#define IDS_OPENFILTERS                 105
#define IDS_ITNFILTERS                  106
#define IDS_GPXFILTERS                  107
#define IDS_ERROPEN                     108
#define IDS_ERRSAVE                     109
#define IDS_ERRITN                      110
#define IDS_RTEFILTERS                  111
#define IDS_BT_EXPORT                   112
#define IDS_BT_OPEN                     113
#define IDS_BT_EXIT                     114
#define IDS_AXEFILTERS                  115
#define IDS_CSVFILTERS                  116
#define IDS_KMLFILTERS                  117
#define IDS_TTMFILTERS                  118
#define IDS_LABEL_EXPORT                119
#define IDS_REMOVE_COMMA                120
#define IDS_CUT_ITN                     121
#define IDS_EXT_AXE                     122
#define IDS_REMOVE_ACCENTS              123
#define IDS_NONE                        124
#define IDS_CUT_ITN_TT3                 125
#define IDS_CUT_ITN_TT5                 126
#define IDS_CUT_ITN_CUSTOM              127
#define IDS_OPTIONS                     128
#define IDS_MN5FILTERS                  129
#define IDS_DATFILTERS                  130
#define IDS_MN4FILTERS                  131
#define IDS_FROM_TO                     132
#define IDS_FREE_133                    133
#define IDS_RDNFILTERS                  134
#define IDS_BCRFILTERS                  135
#define IDS_AXGFILTERS                  136
#define IDS_PTMFILTERS                  137
#define IDS_ESTFILTERS                  138
#define IDS_GDBFILTERS                  139
#define IDS_ALLFILTERS                  140
#define IDS_UP                          141
#define IDS_DOWN                        142
#define IDS_ADD                         143
#define IDS_REMOVE                      144
#define IDS_MODIFY                      145
#define IDS_ERR_LATITUDE                146
#define IDS_ERR_LONGITUDE               147
#define IDS_WAYPOINTS_NB                148
#define IDS_OZIFILTERS                  149
#define IDS_RT2FILTERS                  150
#define IDS_INVERT                      151
#define IDS_MAGFILTERS                  152
#define IDS_EXPLORIST                   153
#define IDS_TEMPLATE                    154
#define IDS_AXTFILTERS                  155
#define IDS_PTTFILTERS                  156
#define IDS_STTFILTERS                  157
#define IDS_TRLFILTERS                  158
#define IDS_URLFILTERS                  159
#define IDS_NAME                        160
#define IDS_FILENAME                    161
#define IDS_ROUTE                       162
#define IDS_UNKNOWN                     163
#define IDS_BADFORMAT                   164
#define IDS_MN6FILTERS                  165
#define IDS_GOOGLE_MAP_VIEW             166
#define IDS_VIRTUAL_EARTH_VIEW          167
#define IDS_MSN_VE_LINK                 168
#define IDS_GOOGLE_MAPS_LINK            169
#define IDS_EDITOR                      170
#define IDS_GOOGLE_LANG                 171
#define IDS_SEARCH                      172
#define IDS_SEARCH_RESULTS              173
#define IDS_RESULTS                     174
#define IDS_ITINERARY                   175
#define IDS_SEARCHING                   176
#define IDS_TOOLTIP_CALCULATE           177
#define IDS_TOOLTIP_VIEWALL             178
#define IDS_NETWORK_ERR                 179
#define IDS_DOWNLOADING                 180
#define IDS_PROVIDER_WAZE_USA           181
#define IDS_PROVIDER_WAZE_WORLD         182
#define IDS_GEO_SERVER_ERROR            183
#define IDS_GEO_MISSING_ADDRESS         184
#define IDS_GEO_UNKNOWN_ADDRESS         185
#define IDS_UNAVAILABLE_ADDRESS         186
#define IDS_NEW_VERSION                 187
#define IDS_TOOLTIP_CLEARROUTE          188
#define IDS_ADVICE                      189
#define IDS_ROUTING_PROGRESS            190
#define IDS_ROUTING_ERROR               191
#define IDS_ROUTING_SUCESS              192
#define IDS_TOOLTIP_REMOVEALL           193
#define IDS_CONFIRM_REMOVE              194
#define IDS_EMPTY                       195
#define IDS_TOOLTIP_OPTIMISE            196
#define IDS_POI                         197
#define IDS_SUMMARY                     198
#define IDS_ZOOM_LEVELS                 199
#define IDS_GPGFILTERS                  200
#define IDS_LOCFILTERS                  201
#define IDS_LOCALE_CSV                  202
#define IDS_LOCALE_SETTINGS             202
#define IDS_GGMFILTERS                  203
#define IDS_PRINT_VERSION               204
#define IDS_CONFIRM_CLOSE               205
#define IDS_SEARCH_NEAR_FROM            206
#define IDS_SEARCH_LOCATION             207
#define IDS_SEARCH_PROXIMITY            208
#define IDS_NO_PROXIMITY                209
#define IDS_CFG_AUTO_CALC               210
#define IDS_CFG_ROUTE_PROVIDER          211
#define IDS_CFG_SETTINGS                212
#define IDS_CFG_DIST_MILES              213
#define IDS_CFG_BACK_MAP                214
#define IDS_CFG_BACK_SAT                215
#define IDS_XVMFILTERS                  216
#define IDS_FAVORITES                   217
#define IDS_MPSFILTERS                  218
#define IDS_TRPFILTERS                  219
#define IDS_SHAREVIOLATION              220
#define IDS_TOOLTIP_VALIDATE            221
#define IDS_ADD_TO_TOMTOM               222
#define IDS_BCKFILTERS                  223
#define IDS_ROUTEFILTERS                224
#define IDS_LMXFILTERS                  225
#define IDS_ALTITUDE                    226
#define IDS_SNIPPET                     227
#define IDS_DEFAULT                     228
#define IDS_INFORAD                     229
#define IDS_COLUMN                      230
#define IDS_LOCALE_SDECIMAL             231
#define IDS_LOCALE_SLIST                232
#define IDS_CFG_AVOID_HIGHWAY           233
#define IDS_CFG_AVOID_TOLLS             234
#define IDS_CFG_STRAIGHT_LINES          235
#define IDS_BING_LANG                   236
#define IDS_CFG_OPT_ITN                 237
#define IDS_CFG_BACKGROUND              238
#define IDS_SEND_TO_NVGFRESH            239
#define IDS_IMPORT_FAV                  240
#define IDS_EXPORT_FAV                  241
#define IDS_TOMTOM_MAP_VIEW             242
#define IDS_TOMTOM_MAPS_LINK            243
#define IDS_APPEND_ROUTE                244
#define IDS_NVGFILTERS                  245
#define IDS_SEND_TO_NAVGATE             246
#define IDS_PIONEER                     247
#define IDS_SEND_TO_MIOMORE             248
#define IDS_NVMFILTERS                  249
#define IDS_ITFFILTERS                  250
#define IDS_DESTINATOR                  252
#define IDS_SEND_TO_GARMIN              253
#define IDS_TRBFILTERS                  254
#define IDS_EXPORT_ITINERARY            255
#define IDS_START_EXPORT                256
#define IDS_SAVE_EXPORT                 257
#define IDS_TOOLTIP_FORCE_ROAD          258
#define IDS_GPLFILTERS                  259
#define IDS_FLKFILTERS                  260
#define IDS_KRTFILTERS                  261
#define IDS_TKFILTERS                   262
#define IDS_CP8FILTERS                  263
#define IDS_TRACK                       264
#define IDS_WAYPOINTS                   265
#define IDS_NOTITLE                     266
#define IDS_ARRAYDESC                   267
#define IDS_CHOOSE_LABEL                268
#define IDS_CHOOSE_TITLE                269
#define IDS_TRIPY_ERROR                 270
#define IDS_TOOMANYPOINTS               271
#define IDS_PLEASEWAIT                  272
#define IDS_TXTFILTERS                  273
#define IDS_CFG_AUTO_ADD_STEP           274
#define IDS_INTERMEDIATE_PROGRESS       275
#define IDS_INTERMEDIATE_SUCESS         276
#define IDS_CFG_BACK_TOPO               277
#define IDS_TOOLTIP_PRINT               278
#define IDS_PROVIDER_GOOGLE_MAP         279
#define IDS_PROVIDER_BING_API           280
#define IDS_PROVIDER_VIAMICHLIN         281
#define IDS_ITINERARY_TYPE              282
#define IDS_ITINERARY_DEFAULT           283
#define IDS_ITINERARY_QUICKEST          284
#define IDS_ITINERARY_SHORTEST          285
#define IDS_ITINERARY_SIGHTSEEING       286
#define IDS_ITINERARY_ECONOMICAL        287
#define IDS_VEHICLE_FOOT                288
#define IDS_VEHICLE_BIKE                289
#define IDS_VEHICLE_TRUCK               290
#define IDS_VIAMICHELIN_LANG            291
#define IDS_PROVIDER_OSM                292
#define IDS_UPOIFILTERS                 293
#define IDS_UPOI80FILTERS               294
#define IDS_UPOI83FILTERS               295
#define IDS_CONFIRM_READ_URL            296
#define IDS_PROVIDER_TRIPY_RTWEB        297
#define IDS_OV2FILTERS                  298
#define IDS_ITF11FILTERS                299
#define IDS_RTFILTERS                   300
#define IDS_PLTFILTERS                  301
#define IDS_COUNTRY_ISOALPHA3           302
#define IDS_NDRIVEFILTERS               303
#define IDS_TOOLTIP_UP                  304
#define IDS_TOOLTIP_DOWN                305
#define IDS_TOOLTIP_ADD                 306
#define IDS_TOOLTIP_REMOVE              307
#define IDS_TOOLTIP_INVERT              308
#define IDS_TOOLTIP_EDITOR              309
#define IDS_TOOLTIP_BT_EXPORT           310
#define IDS_TOOLTIP_BT_EXIT             311
#define IDS_TOOLTIP_BT_OPEN             312
#define IDS_TOOLTIP_CFG_SETTINGS        313
#define IDS_TOOLTIP_SEARCH              314
#define IDS_DECIMAL_DEGREES             315
#define IDS_DEGREES_MINUTES             316
#define IDS_DEGREES_MINUTES_SECONDS     317
#define IDS_COORD_FORMAT                318
#define IDS_MPFCTRFILTERS               319
#define IDS_CFG_OPT_DISPLAY             320
#define IDS_CFG_DISPLAY_NUMBER          321
#define IDS_CFG_BACK_HYBRID             322
#define IDS_TOOLTIP_SELECT_ALL          323
#define IDS_TOOLTIP_UNSELECT_ALL        324
#define IDS_WPTFILTERS                  325
#define IDS_OSMFILTERS                  326
#define IDS_PROVIDER_HERE_API           327
#define IDS_ITF13FILTERS                328
#define IDS_PROVIDER_TOMTOM             329
#define IDS_HERE_MAPS_VIEW              330
#define IDS_HERE_MAPS_LINK              331
#define IDS_HEREFILTERS                 332
#define IDS_ITINERARY_THRILLING         333
#define IDS_VEHICLE_TYPE                334
#define IDS_VEHICLE_CAR                 335
#define IDS_VEHICLE_MOTORBIKE           336
#define IDS_WEB_EDITOR_LANG             337
#define IDS_CFG_TRUCK_HEIGHT            338
#define IDS_CFG_TRUCK_WIDTH             339
#define IDS_CFG_TRUCK_LENGTH            340
#define IDS_CFG_TRUCK_TRAILERS          341
#define IDS_CFG_TRUCK_LIMITED_WEIGHT    342
#define IDS_CFG_TRUCK_WEIGHT_AXLE       343
#define IDS_CFG_TRUCK_CATEGORY          344
#define IDS_TRUCK_NO_CATEGORY           345
#define IDS_TRUCK_PUBLIC_TRANSPORT      346
#define IDS_TRUCK_COMMON_GOODS          347
#define IDS_TRUCK_HAZARDOUS_EXPLOSIVE   348
#define IDS_TRUCK_HAZARDOUS_GAS         349
#define IDS_TRUCK_HAZARDOUS_FLAMMABLE   350
#define IDS_TRUCK_HAZARDOUS_COMBUSTIBLE 351
#define IDS_TRUCK_HAZARDOUS_ORGANIC     352
#define IDS_TRUCK_HAZARDOUS_POISON      353
#define IDS_TRUCK_HAZARDOUS_RADIOACTIVE 354
#define IDS_TRUCK_HAZARDOUS_CORROSIVE   355
#define IDS_TRUCK_HAZARDOUS_POISONOUS   356
#define IDS_TRUCK_HAZARDOUS_HARMFUL     357
#define IDS_TRUCK_HAZARDOUS_OTHER       358
#define IDS_TRUCK_HAZARDOUS_ALL         359
#define IDS_ERR_TRUCK_HEIGHT            360
#define IDS_ERR_TRUCK_WIDTH             361
#define IDS_ERR_TRUCK_LENGTH            362
#define IDS_ERR_TRUCK_LIMITED_WEIGHT    363
#define IDS_ERR_TRUCK_WEIGHT_AXLE       364
#define IDS_VEHICLE_CAMPERVAN           365
#define IDS_VEHICLE_BUS                 366
#define IDS_VMMFILTERS                  367
#define IDS_TTDFILTERS                  368
#define IDS_CUT_ITN_RIDER               369
#define IDS_GPXDAIMLER50FILTERS         370
#define IDS_GPXDAIMLER55FILTERS         371
#define IDS_GPXDAIMLERUBXFILTERS        372
#define IDS_XMLFILTERS                  373
#define IDS_CP10FILTERS                 374
#define IDS_PROVIDER_GEOPORTAL_API      375
#define IDS_GBCFILTERS                  376
#define IDS_CLEAR_FAV                   377
#define IDS_CONFIRM_CLEAR_FAV           378
#define IDS_URLSYGICFILTERS             379
