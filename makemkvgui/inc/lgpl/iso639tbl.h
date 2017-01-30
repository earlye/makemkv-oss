/*
    MakeMKV GUI - Graphics user interface application for MakeMKV

    Written by GuinpinSoft inc <makemkvgui@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/

static const uint8_t l_name_aar[]={'A','f','a','r',0};
static const uint8_t l_name_abk[]={'A','b','k','h','a','z','i','a','n',0};
static const uint8_t l_name_ace[]={'A','c','h','i','n','e','s','e',0};
static const uint8_t l_name_ach[]={'A','c','o','l','i',0};
static const uint8_t l_name_ada[]={'A','d','a','n','g','m','e',0};
static const uint8_t l_name_ady[]={'A','d','y','g','h','e',0};
static const uint8_t l_name_afa[]={'A','f','r','o','-','A','s','i','a','t','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_afh[]={'A','f','r','i','h','i','l','i',0};
static const uint8_t l_name_afr[]={'A','f','r','i','k','a','a','n','s',0};
static const uint8_t l_name_ain[]={'A','i','n','u',0};
static const uint8_t l_name_aka[]={'A','k','a','n',0};
static const uint8_t l_name_akk[]={'A','k','k','a','d','i','a','n',0};
static const uint8_t l_name_alb[]={'A','l','b','a','n','i','a','n',0};
static const uint8_t l_name_ale[]={'A','l','e','u','t',0};
static const uint8_t l_name_alg[]={'A','l','g','o','n','q','u','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_alt[]={'S','o','u','t','h','e','r','n',' ','A','l','t','a','i',0};
static const uint8_t l_name_amh[]={'A','m','h','a','r','i','c',0};
static const uint8_t l_name_ang[]={'E','n','g','l','i','s','h',',',' ','O','l',0};
static const uint8_t l_name_anp[]={'A','n','g','i','k','a',0};
static const uint8_t l_name_apa[]={'A','p','a','c','h','e',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_ara[]={'A','r','a','b','i','c',0};
static const uint8_t l_name_arc[]={'O','f','f','i','c','i','a','l',' ','A','r','a','m','a','i',0};
static const uint8_t l_name_arg[]={'A','r','a','g','o','n','e','s','e',0};
static const uint8_t l_name_arm[]={'A','r','m','e','n','i','a','n',0};
static const uint8_t l_name_arn[]={'M','a','p','u','d','u','n','g','u','n',0};
static const uint8_t l_name_arp[]={'A','r','a','p','a','h','o',0};
static const uint8_t l_name_art[]={'A','r','t','i','f','i','c','i','a','l',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_arw[]={'A','r','a','w','a','k',0};
static const uint8_t l_name_asm[]={'A','s','s','a','m','e','s','e',0};
static const uint8_t l_name_ast[]={'A','s','t','u','r','i','a','n',0};
static const uint8_t l_name_ath[]={'A','t','h','a','p','a','s','c','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_aus[]={'A','u','s','t','r','a','l','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_ava[]={'A','v','a','r','i','c',0};
static const uint8_t l_name_ave[]={'A','v','e','s','t','a','n',0};
static const uint8_t l_name_awa[]={'A','w','a','d','h','i',0};
static const uint8_t l_name_aym[]={'A','y','m','a','r','a',0};
static const uint8_t l_name_aze[]={'A','z','e','r','b','a','i','j','a','n','i',0};
static const uint8_t l_name_bad[]={'B','a','n','d','a',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_bai[]={'B','a','m','i','l','e','k','e',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_bak[]={'B','a','s','h','k','i','r',0};
static const uint8_t l_name_bal[]={'B','a','l','u','c','h','i',0};
static const uint8_t l_name_bam[]={'B','a','m','b','a','r','a',0};
static const uint8_t l_name_ban[]={'B','a','l','i','n','e','s','e',0};
static const uint8_t l_name_baq[]={'B','a','s','q','u','e',0};
static const uint8_t l_name_bas[]={'B','a','s','a',0};
static const uint8_t l_name_bat[]={'B','a','l','t','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_bej[]={'B','e','j','a',0};
static const uint8_t l_name_bel[]={'B','e','l','a','r','u','s','i','a','n',0};
static const uint8_t l_name_bem[]={'B','e','m','b','a',0};
static const uint8_t l_name_ben[]={'B','e','n','g','a','l','i',0};
static const uint8_t l_name_ber[]={'B','e','r','b','e','r',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_bho[]={'B','h','o','j','p','u','r','i',0};
static const uint8_t l_name_bih[]={'B','i','h','a','r','i',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_bik[]={'B','i','k','o','l',0};
static const uint8_t l_name_bin[]={'B','i','n','i',0};
static const uint8_t l_name_bis[]={'B','i','s','l','a','m','a',0};
static const uint8_t l_name_bla[]={'S','i','k','s','i','k','a',0};
static const uint8_t l_name_bnt[]={'B','a','n','t',0};
static const uint8_t l_name_bos[]={'B','o','s','n','i','a','n',0};
static const uint8_t l_name_bra[]={'B','r','a','j',0};
static const uint8_t l_name_bre[]={'B','r','e','t','o','n',0};
static const uint8_t l_name_btk[]={'B','a','t','a','k',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_bua[]={'B','u','r','i','a','t',0};
static const uint8_t l_name_bug[]={'B','u','g','i','n','e','s','e',0};
static const uint8_t l_name_bul[]={'B','u','l','g','a','r','i','a','n',0};
static const uint8_t l_name_bur[]={'B','u','r','m','e','s','e',0};
static const uint8_t l_name_byn[]={'B','l','i','n',0};
static const uint8_t l_name_cad[]={'C','a','d','d','o',0};
static const uint8_t l_name_cai[]={'C','e','n','t','r','a','l',' ','A','m','e','r','i','c','a','n',' ','I','n','d','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_car[]={'G','a','l','i','b','i',' ','C','a','r','i','b',0};
static const uint8_t l_name_cat[]={'C','a','t','a','l','a','n',0};
static const uint8_t l_name_cau[]={'C','a','u','c','a','s','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_ceb[]={'C','e','b','u','a','n','o',0};
static const uint8_t l_name_cel[]={'C','e','l','t','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_cha[]={'C','h','a','m','o','r','r','o',0};
static const uint8_t l_name_chb[]={'C','h','i','b','c','h','a',0};
static const uint8_t l_name_che[]={'C','h','e','c','h','e','n',0};
static const uint8_t l_name_chg[]={'C','h','a','g','a','t','a','i',0};
static const uint8_t l_name_chi[]={'C','h','i','n','e','s','e',0};
static const uint8_t l_name_chk[]={'C','h','u','u','k','e','s','e',0};
static const uint8_t l_name_chm[]={'M','a','r','i',0};
static const uint8_t l_name_chn[]={'C','h','i','n','o','o','k',' ','j','a','r','g','o','n',0};
static const uint8_t l_name_cho[]={'C','h','o','c','t','a','w',0};
static const uint8_t l_name_chp[]={'C','h','i','p','e','w','y','a','n',0};
static const uint8_t l_name_chr[]={'C','h','e','r','o','k','e','e',0};
static const uint8_t l_name_chu[]={'C','h','u','r','c','h',' ','S','l','a','v','i','c',0};
static const uint8_t l_name_chv[]={'C','h','u','v','a','s','h',0};
static const uint8_t l_name_chy[]={'C','h','e','y','e','n','n','e',0};
static const uint8_t l_name_cmc[]={'C','h','a','m','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_cop[]={'C','o','p','t','i','c',0};
static const uint8_t l_name_cor[]={'C','o','r','n','i','s','h',0};
static const uint8_t l_name_cos[]={'C','o','r','s','i','c','a','n',0};
static const uint8_t l_name_cpe[]={'C','r','e','o','l','e','s',' ','a','n','d',' ','p','i','d','g','i','n','s',',',' ','E','n','g','l','i','s','h',' ','b','a','s','e','d',0};
static const uint8_t l_name_cpf[]={'C','r','e','o','l','e','s',' ','a','n','d',' ','p','i','d','g','i','n','s',',',' ','F','r','e','n','c','h','-','b','a','s','e','d',' ',0};
static const uint8_t l_name_cpp[]={'C','r','e','o','l','e','s',' ','a','n','d',' ','p','i','d','g','i','n','s',',',' ','P','o','r','t','u','g','u','e','s','e','-','b','a','s','e','d',' ',0};
static const uint8_t l_name_cre[]={'C','r','e','e',0};
static const uint8_t l_name_crh[]={'C','r','i','m','e','a','n',' ','T','a','t','a','r',0};
static const uint8_t l_name_crp[]={'C','r','e','o','l','e','s',' ','a','n','d',' ','p','i','d','g','i','n','s',' ',0};
static const uint8_t l_name_csb[]={'K','a','s','h','u','b','i','a','n',0};
static const uint8_t l_name_cus[]={'C','u','s','h','i','t','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_cze[]={'C','z','e','c','h',0};
static const uint8_t l_name_dak[]={'D','a','k','o','t','a',0};
static const uint8_t l_name_dan[]={'D','a','n','i','s','h',0};
static const uint8_t l_name_dar[]={'D','a','r','g','w','a',0};
static const uint8_t l_name_day[]={'L','a','n','d',' ','D','a','y','a','k',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_del[]={'D','e','l','a','w','a','r','e',0};
static const uint8_t l_name_den[]={'S','l','a','v',0};
static const uint8_t l_name_dgr[]={'D','o','g','r','i','b',0};
static const uint8_t l_name_din[]={'D','i','n','k','a',0};
static const uint8_t l_name_div[]={'D','i','v','e','h','i',0};
static const uint8_t l_name_doi[]={'D','o','g','r','i',0};
static const uint8_t l_name_dra[]={'D','r','a','v','i','d','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_dsb[]={'L','o','w','e','r',' ','S','o','r','b','i','a','n',0};
static const uint8_t l_name_dua[]={'D','u','a','l','a',0};
static const uint8_t l_name_dum[]={'D','u','t','c','h',',',' ','M','i','d','d','l',0};
static const uint8_t l_name_dut[]={'D','u','t','c','h',0};
static const uint8_t l_name_dyu[]={'D','y','u','l','a',0};
static const uint8_t l_name_dzo[]={'D','z','o','n','g','k','h','a',0};
static const uint8_t l_name_efi[]={'E','f','i','k',0};
static const uint8_t l_name_egy[]={'E','g','y','p','t','i','a',0};
static const uint8_t l_name_eka[]={'E','k','a','j','u','k',0};
static const uint8_t l_name_elx[]={'E','l','a','m','i','t','e',0};
static const uint8_t l_name_eng[]={'E','n','g','l','i','s','h',0};
static const uint8_t l_name_enm[]={'E','n','g','l','i','s','h',',',' ','M','i','d','d','l',0};
static const uint8_t l_name_epo[]={'E','s','p','e','r','a','n','t','o',0};
static const uint8_t l_name_est[]={'E','s','t','o','n','i','a','n',0};
static const uint8_t l_name_ewe[]={'E','w','e',0};
static const uint8_t l_name_ewo[]={'E','w','o','n','d','o',0};
static const uint8_t l_name_fan[]={'F','a','n','g',0};
static const uint8_t l_name_fao[]={'F','a','r','o','e','s','e',0};
static const uint8_t l_name_fat[]={'F','a','n','t','i',0};
static const uint8_t l_name_fij[]={'F','i','j','i','a','n',0};
static const uint8_t l_name_fil[]={'F','i','l','i','p','i','n','o',0};
static const uint8_t l_name_fin[]={'F','i','n','n','i','s','h',0};
static const uint8_t l_name_fiu[]={'F','i','n','n','o','-','U','g','r','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_fon[]={'F','o','n',0};
static const uint8_t l_name_fre[]={'F','r','e','n','c','h',0};
static const uint8_t l_name_frm[]={'F','r','e','n','c','h',',',' ','M','i','d','d','l',0};
static const uint8_t l_name_fro[]={'F','r','e','n','c','h',',',' ','O','l',0};
static const uint8_t l_name_frr[]={'N','o','r','t','h','e','r','n',' ','F','r','i','s','i','a','n',0};
static const uint8_t l_name_frs[]={'E','a','s','t','e','r','n',' ','F','r','i','s','i','a','n',0};
static const uint8_t l_name_fry[]={'W','e','s','t','e','r','n',' ','F','r','i','s','i','a','n',0};
static const uint8_t l_name_ful[]={'F','u','l','a','h',0};
static const uint8_t l_name_fur[]={'F','r','i','u','l','i','a','n',0};
static const uint8_t l_name_gaa[]={'G','a',0};
static const uint8_t l_name_gay[]={'G','a','y','o',0};
static const uint8_t l_name_gba[]={'G','b','a','y','a',0};
static const uint8_t l_name_gem[]={'G','e','r','m','a','n','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_geo[]={'G','e','o','r','g','i','a','n',0};
static const uint8_t l_name_ger[]={'G','e','r','m','a','n',0};
static const uint8_t l_name_gez[]={'G','e','e','z',0};
static const uint8_t l_name_gil[]={'G','i','l','b','e','r','t','e','s','e',0};
static const uint8_t l_name_gla[]={'G','a','e','l','i','c',0};
static const uint8_t l_name_gle[]={'I','r','i','s','h',0};
static const uint8_t l_name_glg[]={'G','a','l','i','c','i','a','n',0};
static const uint8_t l_name_glv[]={'M','a','n','x',0};
static const uint8_t l_name_gmh[]={'G','e','r','m','a','n',',',' ','M','i','d','d','l','e',' ','H','i','g',0};
static const uint8_t l_name_goh[]={'G','e','r','m','a','n',',',' ','O','l','d',' ','H','i','g',0};
static const uint8_t l_name_gon[]={'G','o','n','d','i',0};
static const uint8_t l_name_gor[]={'G','o','r','o','n','t','a','l','o',0};
static const uint8_t l_name_got[]={'G','o','t','h','i','c',0};
static const uint8_t l_name_grb[]={'G','r','e','b','o',0};
static const uint8_t l_name_grc[]={'G','r','e','e','k',',',' ','A','n','c','i','e','n',0};
static const uint8_t l_name_gre[]={'G','r','e','e','k',',',' ','M','o','d','e','r',0};
static const uint8_t l_name_grn[]={'G','u','a','r','a','n','i',0};
static const uint8_t l_name_gsw[]={'S','w','i','s','s',' ','G','e','r','m','a','n',0};
static const uint8_t l_name_guj[]={'G','u','j','a','r','a','t','i',0};
static const uint8_t l_name_gwi[]={'G','w','i','c','h',0x27,'i','n',0};
static const uint8_t l_name_hai[]={'H','a','i','d','a',0};
static const uint8_t l_name_hat[]={'H','a','i','t','i','a','n',0};
static const uint8_t l_name_hau[]={'H','a','u','s','a',0};
static const uint8_t l_name_haw[]={'H','a','w','a','i','i','a','n',0};
static const uint8_t l_name_heb[]={'H','e','b','r','e','w',0};
static const uint8_t l_name_her[]={'H','e','r','e','r','o',0};
static const uint8_t l_name_hil[]={'H','i','l','i','g','a','y','n','o','n',0};
static const uint8_t l_name_him[]={'H','i','m','a','c','h','a','l','i',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_hin[]={'H','i','n','d','i',0};
static const uint8_t l_name_hit[]={'H','i','t','t','i','t','e',0};
static const uint8_t l_name_hmn[]={'H','m','o','n','g',0};
static const uint8_t l_name_hmo[]={'H','i','r','i',' ','M','o','t','u',0};
static const uint8_t l_name_hrv[]={'C','r','o','a','t','i','a','n',0};
static const uint8_t l_name_hsb[]={'U','p','p','e','r',' ','S','o','r','b','i','a','n',0};
static const uint8_t l_name_hun[]={'H','u','n','g','a','r','i','a','n',0};
static const uint8_t l_name_hup[]={'H','u','p','a',0};
static const uint8_t l_name_iba[]={'I','b','a','n',0};
static const uint8_t l_name_ibo[]={'I','g','b','o',0};
static const uint8_t l_name_ice[]={'I','c','e','l','a','n','d','i','c',0};
static const uint8_t l_name_ido[]={'I','d','o',0};
static const uint8_t l_name_iii[]={'S','i','c','h','u','a','n',' ','Y','i',0};
static const uint8_t l_name_ijo[]={'I','j','o',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_iku[]={'I','n','u','k','t','i','t','u','t',0};
static const uint8_t l_name_ile[]={'I','n','t','e','r','l','i','n','g','u','e',0};
static const uint8_t l_name_ilo[]={'I','l','o','k','o',0};
static const uint8_t l_name_ina[]={'I','n','t','e','r','l','i','n','g','u',0};
static const uint8_t l_name_inc[]={'I','n','d','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_ind[]={'I','n','d','o','n','e','s','i','a','n',0};
static const uint8_t l_name_ine[]={'I','n','d','o','-','E','u','r','o','p','e','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_inh[]={'I','n','g','u','s','h',0};
static const uint8_t l_name_ipk[]={'I','n','u','p','i','a','q',0};
static const uint8_t l_name_ira[]={'I','r','a','n','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_iro[]={'I','r','o','q','u','o','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_ita[]={'I','t','a','l','i','a','n',0};
static const uint8_t l_name_jav[]={'J','a','v','a','n','e','s','e',0};
static const uint8_t l_name_jbo[]={'L','o','j','b','a','n',0};
static const uint8_t l_name_jpn[]={'J','a','p','a','n','e','s','e',0};
static const uint8_t l_name_jpr[]={'J','u','d','e','o','-','P','e','r','s','i','a','n',0};
static const uint8_t l_name_jrb[]={'J','u','d','e','o','-','A','r','a','b','i','c',0};
static const uint8_t l_name_kaa[]={'K','a','r','a','-','K','a','l','p','a','k',0};
static const uint8_t l_name_kab[]={'K','a','b','y','l','e',0};
static const uint8_t l_name_kac[]={'K','a','c','h','i','n',0};
static const uint8_t l_name_kal[]={'K','a','l','a','a','l','l','i','s','u','t',0};
static const uint8_t l_name_kam[]={'K','a','m','b','a',0};
static const uint8_t l_name_kan[]={'K','a','n','n','a','d','a',0};
static const uint8_t l_name_kar[]={'K','a','r','e','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_kas[]={'K','a','s','h','m','i','r','i',0};
static const uint8_t l_name_kau[]={'K','a','n','u','r','i',0};
static const uint8_t l_name_kaw[]={'K','a','w','i',0};
static const uint8_t l_name_kaz[]={'K','a','z','a','k','h',0};
static const uint8_t l_name_kbd[]={'K','a','b','a','r','d','i','a','n',0};
static const uint8_t l_name_kha[]={'K','h','a','s','i',0};
static const uint8_t l_name_khi[]={'K','h','o','i','s','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_khm[]={'C','e','n','t','r','a','l',' ','K','h','m','e','r',0};
static const uint8_t l_name_kho[]={'K','h','o','t','a','n','e','s','e',0};
static const uint8_t l_name_kik[]={'K','i','k','u','y','u',0};
static const uint8_t l_name_kin[]={'K','i','n','y','a','r','w','a','n','d','a',0};
static const uint8_t l_name_kir[]={'K','i','r','g','h','i','z',0};
static const uint8_t l_name_kmb[]={'K','i','m','b','u','n','d','u',0};
static const uint8_t l_name_kok[]={'K','o','n','k','a','n','i',0};
static const uint8_t l_name_kom[]={'K','o','m','i',0};
static const uint8_t l_name_kon[]={'K','o','n','g','o',0};
static const uint8_t l_name_kor[]={'K','o','r','e','a','n',0};
static const uint8_t l_name_kos[]={'K','o','s','r','a','e','a','n',0};
static const uint8_t l_name_kpe[]={'K','p','e','l','l','e',0};
static const uint8_t l_name_krc[]={'K','a','r','a','c','h','a','y','-','B','a','l','k','a','r',0};
static const uint8_t l_name_krl[]={'K','a','r','e','l','i','a','n',0};
static const uint8_t l_name_kro[]={'K','r','u',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_kru[]={'K','u','r','u','k','h',0};
static const uint8_t l_name_kua[]={'K','u','a','n','y','a','m','a',0};
static const uint8_t l_name_kum[]={'K','u','m','y','k',0};
static const uint8_t l_name_kur[]={'K','u','r','d','i','s','h',0};
static const uint8_t l_name_kut[]={'K','u','t','e','n','a','i',0};
static const uint8_t l_name_lad[]={'L','a','d','i','n','o',0};
static const uint8_t l_name_lah[]={'L','a','h','n','d','a',0};
static const uint8_t l_name_lam[]={'L','a','m','b','a',0};
static const uint8_t l_name_lao[]={'L','a','o',0};
static const uint8_t l_name_lat[]={'L','a','t','i','n',0};
static const uint8_t l_name_lav[]={'L','a','t','v','i','a','n',0};
static const uint8_t l_name_lez[]={'L','e','z','g','h','i','a','n',0};
static const uint8_t l_name_lim[]={'L','i','m','b','u','r','g','a','n',0};
static const uint8_t l_name_lin[]={'L','i','n','g','a','l','a',0};
static const uint8_t l_name_lit[]={'L','i','t','h','u','a','n','i','a','n',0};
static const uint8_t l_name_lol[]={'M','o','n','g','o',0};
static const uint8_t l_name_loz[]={'L','o','z','i',0};
static const uint8_t l_name_ltz[]={'L','u','x','e','m','b','o','u','r','g','i','s','h',0};
static const uint8_t l_name_lua[]={'L','u','b','a','-','L','u','l','u','a',0};
static const uint8_t l_name_lub[]={'L','u','b','a','-','K','a','t','a','n','g','a',0};
static const uint8_t l_name_lug[]={'G','a','n','d','a',0};
static const uint8_t l_name_lui[]={'L','u','i','s','e','n','o',0};
static const uint8_t l_name_lun[]={'L','u','n','d','a',0};
static const uint8_t l_name_luo[]={'L','u',0};
static const uint8_t l_name_lus[]={'L','u','s','h','a','i',0};
static const uint8_t l_name_mac[]={'M','a','c','e','d','o','n','i','a','n',0};
static const uint8_t l_name_mad[]={'M','a','d','u','r','e','s','e',0};
static const uint8_t l_name_mag[]={'M','a','g','a','h','i',0};
static const uint8_t l_name_mah[]={'M','a','r','s','h','a','l','l','e','s','e',0};
static const uint8_t l_name_mai[]={'M','a','i','t','h','i','l','i',0};
static const uint8_t l_name_mak[]={'M','a','k','a','s','a','r',0};
static const uint8_t l_name_mal[]={'M','a','l','a','y','a','l','a','m',0};
static const uint8_t l_name_man[]={'M','a','n','d','i','n','g','o',0};
static const uint8_t l_name_mao[]={'M','a','o','r','i',0};
static const uint8_t l_name_map[]={'A','u','s','t','r','o','n','e','s','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_mar[]={'M','a','r','a','t','h','i',0};
static const uint8_t l_name_mas[]={'M','a','s','a','i',0};
static const uint8_t l_name_may[]={'M','a','l','a','y',0};
static const uint8_t l_name_mdf[]={'M','o','k','s','h','a',0};
static const uint8_t l_name_mdr[]={'M','a','n','d','a','r',0};
static const uint8_t l_name_men[]={'M','e','n','d','e',0};
static const uint8_t l_name_mga[]={'I','r','i','s','h',',',' ','M','i','d','d','l',0};
static const uint8_t l_name_mic[]={'M','i',0x27,'k','m','a','q',0};
static const uint8_t l_name_min[]={'M','i','n','a','n','g','k','a','b','a','u',0};
static const uint8_t l_name_mis[]={'U','n','c','o','d','e','d',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_mkh[]={'M','o','n','-','K','h','m','e','r',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_mlg[]={'M','a','l','a','g','a','s','y',0};
static const uint8_t l_name_mlt[]={'M','a','l','t','e','s','e',0};
static const uint8_t l_name_mnc[]={'M','a','n','c','h','u',0};
static const uint8_t l_name_mni[]={'M','a','n','i','p','u','r','i',0};
static const uint8_t l_name_mno[]={'M','a','n','o','b','o',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_moh[]={'M','o','h','a','w','k',0};
static const uint8_t l_name_mon[]={'M','o','n','g','o','l','i','a','n',0};
static const uint8_t l_name_mos[]={'M','o','s','s','i',0};
static const uint8_t l_name_mul[]={'M','u','l','t','i','p','l','e',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_mun[]={'M','u','n','d','a',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_mus[]={'C','r','e','e','k',0};
static const uint8_t l_name_mwl[]={'M','i','r','a','n','d','e','s','e',0};
static const uint8_t l_name_mwr[]={'M','a','r','w','a','r','i',0};
static const uint8_t l_name_myn[]={'M','a','y','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_myv[]={'E','r','z','y','a',0};
static const uint8_t l_name_nah[]={'N','a','h','u','a','t','l',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_nai[]={'N','o','r','t','h',' ','A','m','e','r','i','c','a','n',' ','I','n','d','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_nap[]={'N','e','a','p','o','l','i','t','a','n',0};
static const uint8_t l_name_nau[]={'N','a','u','r','u',0};
static const uint8_t l_name_nav[]={'N','a','v','a','j','o',0};
static const uint8_t l_name_nbl[]={'N','d','e','b','e','l','e',',',' ','S','o','u','t','h',0};
static const uint8_t l_name_nde[]={'N','d','e','b','e','l','e',',',' ','N','o','r','t','h',0};
static const uint8_t l_name_ndo[]={'N','d','o','n','g','a',0};
static const uint8_t l_name_nds[]={'L','o','w',' ','G','e','r','m','a','n',0};
static const uint8_t l_name_nep[]={'N','e','p','a','l','i',0};
static const uint8_t l_name_new[]={'N','e','p','a','l',' ','B','h','a','s','a',0};
static const uint8_t l_name_nia[]={'N','i','a','s',0};
static const uint8_t l_name_nic[]={'N','i','g','e','r','-','K','o','r','d','o','f','a','n','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_niu[]={'N','i','u','e','a','n',0};
static const uint8_t l_name_nno[]={'N','o','r','w','e','g','i','a','n',' ','N','y','n','o','r','s','k',0};
static const uint8_t l_name_nob[]={'B','o','k','m',0xc3,0xa5,'l',',',' ','N','o','r','w','e','g','i','a','n',0};
static const uint8_t l_name_nog[]={'N','o','g','a','i',0};
static const uint8_t l_name_non[]={'N','o','r','s','e',',',' ','O','l','d',0};
static const uint8_t l_name_nor[]={'N','o','r','w','e','g','i','a','n',0};
static const uint8_t l_name_nqo[]={'N',0x27,'K','o',0};
static const uint8_t l_name_nso[]={'P','e','d','i',0};
static const uint8_t l_name_nub[]={'N','u','b','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_nwc[]={'C','l','a','s','s','i','c','a','l',' ','N','e','w','a','r','i',0};
static const uint8_t l_name_nya[]={'C','h','i','c','h','e','w','a',0};
static const uint8_t l_name_nym[]={'N','y','a','m','w','e','z','i',0};
static const uint8_t l_name_nyn[]={'N','y','a','n','k','o','l','e',0};
static const uint8_t l_name_nyo[]={'N','y','o','r','o',0};
static const uint8_t l_name_nzi[]={'N','z','i','m','a',0};
static const uint8_t l_name_oci[]={'O','c','c','i','t','a',0};
static const uint8_t l_name_oji[]={'O','j','i','b','w','a',0};
static const uint8_t l_name_ori[]={'O','r','i','y','a',0};
static const uint8_t l_name_orm[]={'O','r','o','m','o',0};
static const uint8_t l_name_osa[]={'O','s','a','g','e',0};
static const uint8_t l_name_oss[]={'O','s','s','e','t','i','a','n',0};
static const uint8_t l_name_ota[]={'T','u','r','k','i','s','h',',',' ','O','t','t','o','m','a',0};
static const uint8_t l_name_oto[]={'O','t','o','m','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_paa[]={'P','a','p','u','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_pag[]={'P','a','n','g','a','s','i','n','a','n',0};
static const uint8_t l_name_pal[]={'P','a','h','l','a','v','i',0};
static const uint8_t l_name_pam[]={'P','a','m','p','a','n','g','a',0};
static const uint8_t l_name_pan[]={'P','a','n','j','a','b','i',0};
static const uint8_t l_name_pap[]={'P','a','p','i','a','m','e','n','t','o',0};
static const uint8_t l_name_pau[]={'P','a','l','a','u','a','n',0};
static const uint8_t l_name_peo[]={'P','e','r','s','i','a','n',',',' ','O','l',0};
static const uint8_t l_name_per[]={'P','e','r','s','i','a','n',0};
static const uint8_t l_name_phi[]={'P','h','i','l','i','p','p','i','n','e',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_phn[]={'P','h','o','e','n','i','c','i','a','n',0};
static const uint8_t l_name_pli[]={'P','a','l','i',0};
static const uint8_t l_name_pol[]={'P','o','l','i','s','h',0};
static const uint8_t l_name_pon[]={'P','o','h','n','p','e','i','a','n',0};
static const uint8_t l_name_por[]={'P','o','r','t','u','g','u','e','s','e',0};
static const uint8_t l_name_pra[]={'P','r','a','k','r','i','t',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_pro[]={'P','r','o','v','e','n',0xc3,0xa7,'a','l',',',' ','O','l',0};
static const uint8_t l_name_pus[]={'P','u','s','h','t','o',0};
static const uint8_t l_name_que[]={'Q','u','e','c','h','u','a',0};
static const uint8_t l_name_raj[]={'R','a','j','a','s','t','h','a','n','i',0};
static const uint8_t l_name_rap[]={'R','a','p','a','n','u','i',0};
static const uint8_t l_name_rar[]={'R','a','r','o','t','o','n','g','a','n',0};
static const uint8_t l_name_roa[]={'R','o','m','a','n','c','e',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_roh[]={'R','o','m','a','n','s','h',0};
static const uint8_t l_name_rom[]={'R','o','m','a','n','y',0};
static const uint8_t l_name_rum[]={'R','o','m','a','n','i','a','n',0};
static const uint8_t l_name_run[]={'R','u','n','d','i',0};
static const uint8_t l_name_rup[]={'A','r','o','m','a','n','i','a','n',0};
static const uint8_t l_name_rus[]={'R','u','s','s','i','a','n',0};
static const uint8_t l_name_sad[]={'S','a','n','d','a','w','e',0};
static const uint8_t l_name_sag[]={'S','a','n','g','o',0};
static const uint8_t l_name_sah[]={'Y','a','k','u','t',0};
static const uint8_t l_name_sai[]={'S','o','u','t','h',' ','A','m','e','r','i','c','a','n',' ','I','n','d','i','a',0};
static const uint8_t l_name_sal[]={'S','a','l','i','s','h','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_sam[]={'S','a','m','a','r','i','t','a','n',' ','A','r','a','m','a','i','c',0};
static const uint8_t l_name_san[]={'S','a','n','s','k','r','i','t',0};
static const uint8_t l_name_sas[]={'S','a','s','a','k',0};
static const uint8_t l_name_sat[]={'S','a','n','t','a','l','i',0};
static const uint8_t l_name_scn[]={'S','i','c','i','l','i','a','n',0};
static const uint8_t l_name_sco[]={'S','c','o','t','s',0};
static const uint8_t l_name_sel[]={'S','e','l','k','u','p',0};
static const uint8_t l_name_sem[]={'S','e','m','i','t','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_sga[]={'I','r','i','s','h',',',' ','O','l',0};
static const uint8_t l_name_sgn[]={'S','i','g','n',' ','L','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_shn[]={'S','h','a','n',0};
static const uint8_t l_name_sid[]={'S','i','d','a','m','o',0};
static const uint8_t l_name_sin[]={'S','i','n','h','a','l','a',0};
static const uint8_t l_name_sio[]={'S','i','o','u','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_sit[]={'S','i','n','o','-','T','i','b','e','t','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_sla[]={'S','l','a','v','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_slo[]={'S','l','o','v','a','k',0};
static const uint8_t l_name_slv[]={'S','l','o','v','e','n','i','a','n',0};
static const uint8_t l_name_sma[]={'S','o','u','t','h','e','r','n',' ','S','a','m','i',0};
static const uint8_t l_name_sme[]={'N','o','r','t','h','e','r','n',' ','S','a','m','i',0};
static const uint8_t l_name_smi[]={'S','a','m','i',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_smj[]={'L','u','l','e',' ','S','a','m','i',0};
static const uint8_t l_name_smn[]={'I','n','a','r','i',' ','S','a','m','i',0};
static const uint8_t l_name_smo[]={'S','a','m','o','a','n',0};
static const uint8_t l_name_sms[]={'S','k','o','l','t',' ','S','a','m','i',0};
static const uint8_t l_name_sna[]={'S','h','o','n','a',0};
static const uint8_t l_name_snd[]={'S','i','n','d','h','i',0};
static const uint8_t l_name_snk[]={'S','o','n','i','n','k','e',0};
static const uint8_t l_name_sog[]={'S','o','g','d','i','a','n',0};
static const uint8_t l_name_som[]={'S','o','m','a','l','i',0};
static const uint8_t l_name_son[]={'S','o','n','g','h','a','i',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_sot[]={'S','o','t','h','o',',',' ','S','o','u','t','h','e','r','n',0};
static const uint8_t l_name_spa[]={'S','p','a','n','i','s','h',0};
static const uint8_t l_name_srd[]={'S','a','r','d','i','n','i','a','n',0};
static const uint8_t l_name_srn[]={'S','r','a','n','a','n',' ','T','o','n','g','o',0};
static const uint8_t l_name_srp[]={'S','e','r','b','i','a','n',0};
static const uint8_t l_name_srr[]={'S','e','r','e','r',0};
static const uint8_t l_name_ssa[]={'N','i','l','o','-','S','a','h','a','r','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_ssw[]={'S','w','a','t','i',0};
static const uint8_t l_name_suk[]={'S','u','k','u','m','a',0};
static const uint8_t l_name_sun[]={'S','u','n','d','a','n','e','s','e',0};
static const uint8_t l_name_sus[]={'S','u','s','u',0};
static const uint8_t l_name_sux[]={'S','u','m','e','r','i','a','n',0};
static const uint8_t l_name_swa[]={'S','w','a','h','i','l','i',0};
static const uint8_t l_name_swe[]={'S','w','e','d','i','s','h',0};
static const uint8_t l_name_syc[]={'C','l','a','s','s','i','c','a','l',' ','S','y','r','i','a','c',0};
static const uint8_t l_name_syr[]={'S','y','r','i','a','c',0};
static const uint8_t l_name_tah[]={'T','a','h','i','t','i','a','n',0};
static const uint8_t l_name_tai[]={'T','a','i',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_tam[]={'T','a','m','i','l',0};
static const uint8_t l_name_tat[]={'T','a','t','a','r',0};
static const uint8_t l_name_tel[]={'T','e','l','u','g','u',0};
static const uint8_t l_name_tem[]={'T','i','m','n','e',0};
static const uint8_t l_name_ter[]={'T','e','r','e','n','o',0};
static const uint8_t l_name_tet[]={'T','e','t','u','m',0};
static const uint8_t l_name_tgk[]={'T','a','j','i','k',0};
static const uint8_t l_name_tgl[]={'T','a','g','a','l','o','g',0};
static const uint8_t l_name_tha[]={'T','h','a','i',0};
static const uint8_t l_name_tib[]={'T','i','b','e','t','a','n',0};
static const uint8_t l_name_tig[]={'T','i','g','r','e',0};
static const uint8_t l_name_tir[]={'T','i','g','r','i','n','y','a',0};
static const uint8_t l_name_tiv[]={'T','i','v',0};
static const uint8_t l_name_tkl[]={'T','o','k','e','l','a','u',0};
static const uint8_t l_name_tlh[]={'K','l','i','n','g','o','n',0};
static const uint8_t l_name_tli[]={'T','l','i','n','g','i','t',0};
static const uint8_t l_name_tmh[]={'T','a','m','a','s','h','e','k',0};
static const uint8_t l_name_tog[]={'T','o','n','g',0};
static const uint8_t l_name_ton[]={'T','o','n','g',0};
static const uint8_t l_name_tpi[]={'T','o','k',' ','P','i','s','i','n',0};
static const uint8_t l_name_tsi[]={'T','s','i','m','s','h','i','a','n',0};
static const uint8_t l_name_tsn[]={'T','s','w','a','n','a',0};
static const uint8_t l_name_tso[]={'T','s','o','n','g','a',0};
static const uint8_t l_name_tuk[]={'T','u','r','k','m','e','n',0};
static const uint8_t l_name_tum[]={'T','u','m','b','u','k','a',0};
static const uint8_t l_name_tup[]={'T','u','p','i',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_tur[]={'T','u','r','k','i','s','h',0};
static const uint8_t l_name_tut[]={'A','l','t','a','i','c',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_tvl[]={'T','u','v','a','l','u',0};
static const uint8_t l_name_twi[]={'T','w','i',0};
static const uint8_t l_name_tyv[]={'T','u','v','i','n','i','a','n',0};
static const uint8_t l_name_udm[]={'U','d','m','u','r','t',0};
static const uint8_t l_name_uga[]={'U','g','a','r','i','t','i','c',0};
static const uint8_t l_name_uig[]={'U','i','g','h','u','r',0};
static const uint8_t l_name_ukr[]={'U','k','r','a','i','n','i','a','n',0};
static const uint8_t l_name_umb[]={'U','m','b','u','n','d','u',0};
static const uint8_t l_name_und[]={'U','n','d','e','t','e','r','m','i','n','e','d',0};
static const uint8_t l_name_urd[]={'U','r','d','u',0};
static const uint8_t l_name_uzb[]={'U','z','b','e','k',0};
static const uint8_t l_name_vai[]={'V','a','i',0};
static const uint8_t l_name_ven[]={'V','e','n','d','a',0};
static const uint8_t l_name_vie[]={'V','i','e','t','n','a','m','e','s','e',0};
static const uint8_t l_name_vol[]={'V','o','l','a','p',0xc3,0xbc,'k',0};
static const uint8_t l_name_vot[]={'V','o','t','i','c',0};
static const uint8_t l_name_wak[]={'W','a','k','a','s','h','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_wal[]={'W','a','l','a','m','o',0};
static const uint8_t l_name_war[]={'W','a','r','a','y',0};
static const uint8_t l_name_was[]={'W','a','s','h','o',0};
static const uint8_t l_name_wel[]={'W','e','l','s','h',0};
static const uint8_t l_name_wen[]={'S','o','r','b','i','a','n',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_wln[]={'W','a','l','l','o','o','n',0};
static const uint8_t l_name_wol[]={'W','o','l','o','f',0};
static const uint8_t l_name_xal[]={'K','a','l','m','y','k',0};
static const uint8_t l_name_xho[]={'X','h','o','s','a',0};
static const uint8_t l_name_yao[]={'Y','a','o',0};
static const uint8_t l_name_yap[]={'Y','a','p','e','s','e',0};
static const uint8_t l_name_yid[]={'Y','i','d','d','i','s','h',0};
static const uint8_t l_name_yor[]={'Y','o','r','u','b','a',0};
static const uint8_t l_name_ypk[]={'Y','u','p','i','k',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_zap[]={'Z','a','p','o','t','e','c',0};
static const uint8_t l_name_zbl[]={'B','l','i','s','s','y','m','b','o','l','s',0};
static const uint8_t l_name_zen[]={'Z','e','n','a','g','a',0};
static const uint8_t l_name_zha[]={'Z','h','u','a','n','g',0};
static const uint8_t l_name_znd[]={'Z','a','n','d','e',' ','l','a','n','g','u','a','g','e','s',0};
static const uint8_t l_name_zul[]={'Z','u','l','u',0};
static const uint8_t l_name_zun[]={'Z','u','n','i',0};
static const uint8_t l_name_zxx[]={'N','o',' ','l','i','n','g','u','i','s','t','i','c',' ','c','o','n','t','e','n','t',0};
static const uint8_t l_name_zza[]={'Z','a','z','a',0};



static const lang_info_t lang_table[484]={
 { "aar" , "aa" , NULL  , (const char*) l_name_aar } ,
 { "abk" , "ab" , NULL  , (const char*) l_name_abk } ,
 { "ace" , NULL , NULL  , (const char*) l_name_ace } ,
 { "ach" , NULL , NULL  , (const char*) l_name_ach } ,
 { "ada" , NULL , NULL  , (const char*) l_name_ada } ,
 { "ady" , NULL , NULL  , (const char*) l_name_ady } ,
 { "afa" , NULL , NULL  , (const char*) l_name_afa } ,
 { "afh" , NULL , NULL  , (const char*) l_name_afh } ,
 { "afr" , "af" , NULL  , (const char*) l_name_afr } ,
 { "ain" , NULL , NULL  , (const char*) l_name_ain } ,
 { "aka" , "ak" , NULL  , (const char*) l_name_aka } ,
 { "akk" , NULL , NULL  , (const char*) l_name_akk } ,
 { "alb" , "sq" , "sqi" , (const char*) l_name_alb } ,
 { "ale" , NULL , NULL  , (const char*) l_name_ale } ,
 { "alg" , NULL , NULL  , (const char*) l_name_alg } ,
 { "alt" , NULL , NULL  , (const char*) l_name_alt } ,
 { "amh" , "am" , NULL  , (const char*) l_name_amh } ,
 { "ang" , NULL , NULL  , (const char*) l_name_ang } ,
 { "anp" , NULL , NULL  , (const char*) l_name_anp } ,
 { "apa" , NULL , NULL  , (const char*) l_name_apa } ,
 { "ara" , "ar" , NULL  , (const char*) l_name_ara } ,
 { "arc" , NULL , NULL  , (const char*) l_name_arc } ,
 { "arg" , "an" , NULL  , (const char*) l_name_arg } ,
 { "arm" , "hy" , "hye" , (const char*) l_name_arm } ,
 { "arn" , NULL , NULL  , (const char*) l_name_arn } ,
 { "arp" , NULL , NULL  , (const char*) l_name_arp } ,
 { "art" , NULL , NULL  , (const char*) l_name_art } ,
 { "arw" , NULL , NULL  , (const char*) l_name_arw } ,
 { "asm" , "as" , NULL  , (const char*) l_name_asm } ,
 { "ast" , NULL , NULL  , (const char*) l_name_ast } ,
 { "ath" , NULL , NULL  , (const char*) l_name_ath } ,
 { "aus" , NULL , NULL  , (const char*) l_name_aus } ,
 { "ava" , "av" , NULL  , (const char*) l_name_ava } ,
 { "ave" , "ae" , NULL  , (const char*) l_name_ave } ,
 { "awa" , NULL , NULL  , (const char*) l_name_awa } ,
 { "aym" , "ay" , NULL  , (const char*) l_name_aym } ,
 { "aze" , "az" , NULL  , (const char*) l_name_aze } ,
 { "bad" , NULL , NULL  , (const char*) l_name_bad } ,
 { "bai" , NULL , NULL  , (const char*) l_name_bai } ,
 { "bak" , "ba" , NULL  , (const char*) l_name_bak } ,
 { "bal" , NULL , NULL  , (const char*) l_name_bal } ,
 { "bam" , "bm" , NULL  , (const char*) l_name_bam } ,
 { "ban" , NULL , NULL  , (const char*) l_name_ban } ,
 { "baq" , "eu" , "eus" , (const char*) l_name_baq } ,
 { "bas" , NULL , NULL  , (const char*) l_name_bas } ,
 { "bat" , NULL , NULL  , (const char*) l_name_bat } ,
 { "bej" , NULL , NULL  , (const char*) l_name_bej } ,
 { "bel" , "be" , NULL  , (const char*) l_name_bel } ,
 { "bem" , NULL , NULL  , (const char*) l_name_bem } ,
 { "ben" , "bn" , NULL  , (const char*) l_name_ben } ,
 { "ber" , NULL , NULL  , (const char*) l_name_ber } ,
 { "bho" , NULL , NULL  , (const char*) l_name_bho } ,
 { "bih" , "bh" , NULL  , (const char*) l_name_bih } ,
 { "bik" , NULL , NULL  , (const char*) l_name_bik } ,
 { "bin" , NULL , NULL  , (const char*) l_name_bin } ,
 { "bis" , "bi" , NULL  , (const char*) l_name_bis } ,
 { "bla" , NULL , NULL  , (const char*) l_name_bla } ,
 { "bnt" , NULL , NULL  , (const char*) l_name_bnt } ,
 { "bos" , "bs" , NULL  , (const char*) l_name_bos } ,
 { "bra" , NULL , NULL  , (const char*) l_name_bra } ,
 { "bre" , "br" , NULL  , (const char*) l_name_bre } ,
 { "btk" , NULL , NULL  , (const char*) l_name_btk } ,
 { "bua" , NULL , NULL  , (const char*) l_name_bua } ,
 { "bug" , NULL , NULL  , (const char*) l_name_bug } ,
 { "bul" , "bg" , NULL  , (const char*) l_name_bul } ,
 { "bur" , "my" , "mya" , (const char*) l_name_bur } ,
 { "byn" , NULL , NULL  , (const char*) l_name_byn } ,
 { "cad" , NULL , NULL  , (const char*) l_name_cad } ,
 { "cai" , NULL , NULL  , (const char*) l_name_cai } ,
 { "car" , NULL , NULL  , (const char*) l_name_car } ,
 { "cat" , "ca" , NULL  , (const char*) l_name_cat } ,
 { "cau" , NULL , NULL  , (const char*) l_name_cau } ,
 { "ceb" , NULL , NULL  , (const char*) l_name_ceb } ,
 { "cel" , NULL , NULL  , (const char*) l_name_cel } ,
 { "cha" , "ch" , NULL  , (const char*) l_name_cha } ,
 { "chb" , NULL , NULL  , (const char*) l_name_chb } ,
 { "che" , "ce" , NULL  , (const char*) l_name_che } ,
 { "chg" , NULL , NULL  , (const char*) l_name_chg } ,
 { "chi" , "zh" , "zho" , (const char*) l_name_chi } ,
 { "chk" , NULL , NULL  , (const char*) l_name_chk } ,
 { "chm" , NULL , NULL  , (const char*) l_name_chm } ,
 { "chn" , NULL , NULL  , (const char*) l_name_chn } ,
 { "cho" , NULL , NULL  , (const char*) l_name_cho } ,
 { "chp" , NULL , NULL  , (const char*) l_name_chp } ,
 { "chr" , NULL , NULL  , (const char*) l_name_chr } ,
 { "chu" , "cu" , NULL  , (const char*) l_name_chu } ,
 { "chv" , "cv" , NULL  , (const char*) l_name_chv } ,
 { "chy" , NULL , NULL  , (const char*) l_name_chy } ,
 { "cmc" , NULL , NULL  , (const char*) l_name_cmc } ,
 { "cop" , NULL , NULL  , (const char*) l_name_cop } ,
 { "cor" , "kw" , NULL  , (const char*) l_name_cor } ,
 { "cos" , "co" , NULL  , (const char*) l_name_cos } ,
 { "cpe" , NULL , NULL  , (const char*) l_name_cpe } ,
 { "cpf" , NULL , NULL  , (const char*) l_name_cpf } ,
 { "cpp" , NULL , NULL  , (const char*) l_name_cpp } ,
 { "cre" , "cr" , NULL  , (const char*) l_name_cre } ,
 { "crh" , NULL , NULL  , (const char*) l_name_crh } ,
 { "crp" , NULL , NULL  , (const char*) l_name_crp } ,
 { "csb" , NULL , NULL  , (const char*) l_name_csb } ,
 { "cus" , NULL , NULL  , (const char*) l_name_cus } ,
 { "cze" , "cs" , "ces" , (const char*) l_name_cze } ,
 { "dak" , NULL , NULL  , (const char*) l_name_dak } ,
 { "dan" , "da" , NULL  , (const char*) l_name_dan } ,
 { "dar" , NULL , NULL  , (const char*) l_name_dar } ,
 { "day" , NULL , NULL  , (const char*) l_name_day } ,
 { "del" , NULL , NULL  , (const char*) l_name_del } ,
 { "den" , NULL , NULL  , (const char*) l_name_den } ,
 { "dgr" , NULL , NULL  , (const char*) l_name_dgr } ,
 { "din" , NULL , NULL  , (const char*) l_name_din } ,
 { "div" , "dv" , NULL  , (const char*) l_name_div } ,
 { "doi" , NULL , NULL  , (const char*) l_name_doi } ,
 { "dra" , NULL , NULL  , (const char*) l_name_dra } ,
 { "dsb" , NULL , NULL  , (const char*) l_name_dsb } ,
 { "dua" , NULL , NULL  , (const char*) l_name_dua } ,
 { "dum" , NULL , NULL  , (const char*) l_name_dum } ,
 { "dut" , "nl" , "nld" , (const char*) l_name_dut } ,
 { "dyu" , NULL , NULL  , (const char*) l_name_dyu } ,
 { "dzo" , "dz" , NULL  , (const char*) l_name_dzo } ,
 { "efi" , NULL , NULL  , (const char*) l_name_efi } ,
 { "egy" , NULL , NULL  , (const char*) l_name_egy } ,
 { "eka" , NULL , NULL  , (const char*) l_name_eka } ,
 { "elx" , NULL , NULL  , (const char*) l_name_elx } ,
 { "eng" , "en" , NULL  , (const char*) l_name_eng } ,
 { "enm" , NULL , NULL  , (const char*) l_name_enm } ,
 { "epo" , "eo" , NULL  , (const char*) l_name_epo } ,
 { "est" , "et" , NULL  , (const char*) l_name_est } ,
 { "ewe" , "ee" , NULL  , (const char*) l_name_ewe } ,
 { "ewo" , NULL , NULL  , (const char*) l_name_ewo } ,
 { "fan" , NULL , NULL  , (const char*) l_name_fan } ,
 { "fao" , "fo" , NULL  , (const char*) l_name_fao } ,
 { "fat" , NULL , NULL  , (const char*) l_name_fat } ,
 { "fij" , "fj" , NULL  , (const char*) l_name_fij } ,
 { "fil" , NULL , NULL  , (const char*) l_name_fil } ,
 { "fin" , "fi" , NULL  , (const char*) l_name_fin } ,
 { "fiu" , NULL , NULL  , (const char*) l_name_fiu } ,
 { "fon" , NULL , NULL  , (const char*) l_name_fon } ,
 { "fre" , "fr" , "fra" , (const char*) l_name_fre } ,
 { "frm" , NULL , NULL  , (const char*) l_name_frm } ,
 { "fro" , NULL , NULL  , (const char*) l_name_fro } ,
 { "frr" , NULL , NULL  , (const char*) l_name_frr } ,
 { "frs" , NULL , NULL  , (const char*) l_name_frs } ,
 { "fry" , "fy" , NULL  , (const char*) l_name_fry } ,
 { "ful" , "ff" , NULL  , (const char*) l_name_ful } ,
 { "fur" , NULL , NULL  , (const char*) l_name_fur } ,
 { "gaa" , NULL , NULL  , (const char*) l_name_gaa } ,
 { "gay" , NULL , NULL  , (const char*) l_name_gay } ,
 { "gba" , NULL , NULL  , (const char*) l_name_gba } ,
 { "gem" , NULL , NULL  , (const char*) l_name_gem } ,
 { "geo" , "ka" , "kat" , (const char*) l_name_geo } ,
 { "ger" , "de" , "deu" , (const char*) l_name_ger } ,
 { "gez" , NULL , NULL  , (const char*) l_name_gez } ,
 { "gil" , NULL , NULL  , (const char*) l_name_gil } ,
 { "gla" , "gd" , NULL  , (const char*) l_name_gla } ,
 { "gle" , "ga" , NULL  , (const char*) l_name_gle } ,
 { "glg" , "gl" , NULL  , (const char*) l_name_glg } ,
 { "glv" , "gv" , NULL  , (const char*) l_name_glv } ,
 { "gmh" , NULL , NULL  , (const char*) l_name_gmh } ,
 { "goh" , NULL , NULL  , (const char*) l_name_goh } ,
 { "gon" , NULL , NULL  , (const char*) l_name_gon } ,
 { "gor" , NULL , NULL  , (const char*) l_name_gor } ,
 { "got" , NULL , NULL  , (const char*) l_name_got } ,
 { "grb" , NULL , NULL  , (const char*) l_name_grb } ,
 { "grc" , NULL , NULL  , (const char*) l_name_grc } ,
 { "gre" , "el" , "ell" , (const char*) l_name_gre } ,
 { "grn" , "gn" , NULL  , (const char*) l_name_grn } ,
 { "gsw" , NULL , NULL  , (const char*) l_name_gsw } ,
 { "guj" , "gu" , NULL  , (const char*) l_name_guj } ,
 { "gwi" , NULL , NULL  , (const char*) l_name_gwi } ,
 { "hai" , NULL , NULL  , (const char*) l_name_hai } ,
 { "hat" , "ht" , NULL  , (const char*) l_name_hat } ,
 { "hau" , "ha" , NULL  , (const char*) l_name_hau } ,
 { "haw" , NULL , NULL  , (const char*) l_name_haw } ,
 { "heb" , "he" , NULL  , (const char*) l_name_heb } ,
 { "her" , "hz" , NULL  , (const char*) l_name_her } ,
 { "hil" , NULL , NULL  , (const char*) l_name_hil } ,
 { "him" , NULL , NULL  , (const char*) l_name_him } ,
 { "hin" , "hi" , NULL  , (const char*) l_name_hin } ,
 { "hit" , NULL , NULL  , (const char*) l_name_hit } ,
 { "hmn" , NULL , NULL  , (const char*) l_name_hmn } ,
 { "hmo" , "ho" , NULL  , (const char*) l_name_hmo } ,
 { "hrv" , "hr" , NULL  , (const char*) l_name_hrv } ,
 { "hsb" , NULL , NULL  , (const char*) l_name_hsb } ,
 { "hun" , "hu" , NULL  , (const char*) l_name_hun } ,
 { "hup" , NULL , NULL  , (const char*) l_name_hup } ,
 { "iba" , NULL , NULL  , (const char*) l_name_iba } ,
 { "ibo" , "ig" , NULL  , (const char*) l_name_ibo } ,
 { "ice" , "is" , "isl" , (const char*) l_name_ice } ,
 { "ido" , "io" , NULL  , (const char*) l_name_ido } ,
 { "iii" , "ii" , NULL  , (const char*) l_name_iii } ,
 { "ijo" , NULL , NULL  , (const char*) l_name_ijo } ,
 { "iku" , "iu" , NULL  , (const char*) l_name_iku } ,
 { "ile" , "ie" , NULL  , (const char*) l_name_ile } ,
 { "ilo" , NULL , NULL  , (const char*) l_name_ilo } ,
 { "ina" , "ia" , NULL  , (const char*) l_name_ina } ,
 { "inc" , NULL , NULL  , (const char*) l_name_inc } ,
 { "ind" , "id" , NULL  , (const char*) l_name_ind } ,
 { "ine" , NULL , NULL  , (const char*) l_name_ine } ,
 { "inh" , NULL , NULL  , (const char*) l_name_inh } ,
 { "ipk" , "ik" , NULL  , (const char*) l_name_ipk } ,
 { "ira" , NULL , NULL  , (const char*) l_name_ira } ,
 { "iro" , NULL , NULL  , (const char*) l_name_iro } ,
 { "ita" , "it" , NULL  , (const char*) l_name_ita } ,
 { "jav" , "jv" , NULL  , (const char*) l_name_jav } ,
 { "jbo" , NULL , NULL  , (const char*) l_name_jbo } ,
 { "jpn" , "ja" , NULL  , (const char*) l_name_jpn } ,
 { "jpr" , NULL , NULL  , (const char*) l_name_jpr } ,
 { "jrb" , NULL , NULL  , (const char*) l_name_jrb } ,
 { "kaa" , NULL , NULL  , (const char*) l_name_kaa } ,
 { "kab" , NULL , NULL  , (const char*) l_name_kab } ,
 { "kac" , NULL , NULL  , (const char*) l_name_kac } ,
 { "kal" , "kl" , NULL  , (const char*) l_name_kal } ,
 { "kam" , NULL , NULL  , (const char*) l_name_kam } ,
 { "kan" , "kn" , NULL  , (const char*) l_name_kan } ,
 { "kar" , NULL , NULL  , (const char*) l_name_kar } ,
 { "kas" , "ks" , NULL  , (const char*) l_name_kas } ,
 { "kau" , "kr" , NULL  , (const char*) l_name_kau } ,
 { "kaw" , NULL , NULL  , (const char*) l_name_kaw } ,
 { "kaz" , "kk" , NULL  , (const char*) l_name_kaz } ,
 { "kbd" , NULL , NULL  , (const char*) l_name_kbd } ,
 { "kha" , NULL , NULL  , (const char*) l_name_kha } ,
 { "khi" , NULL , NULL  , (const char*) l_name_khi } ,
 { "khm" , "km" , NULL  , (const char*) l_name_khm } ,
 { "kho" , NULL , NULL  , (const char*) l_name_kho } ,
 { "kik" , "ki" , NULL  , (const char*) l_name_kik } ,
 { "kin" , "rw" , NULL  , (const char*) l_name_kin } ,
 { "kir" , "ky" , NULL  , (const char*) l_name_kir } ,
 { "kmb" , NULL , NULL  , (const char*) l_name_kmb } ,
 { "kok" , NULL , NULL  , (const char*) l_name_kok } ,
 { "kom" , "kv" , NULL  , (const char*) l_name_kom } ,
 { "kon" , "kg" , NULL  , (const char*) l_name_kon } ,
 { "kor" , "ko" , NULL  , (const char*) l_name_kor } ,
 { "kos" , NULL , NULL  , (const char*) l_name_kos } ,
 { "kpe" , NULL , NULL  , (const char*) l_name_kpe } ,
 { "krc" , NULL , NULL  , (const char*) l_name_krc } ,
 { "krl" , NULL , NULL  , (const char*) l_name_krl } ,
 { "kro" , NULL , NULL  , (const char*) l_name_kro } ,
 { "kru" , NULL , NULL  , (const char*) l_name_kru } ,
 { "kua" , "kj" , NULL  , (const char*) l_name_kua } ,
 { "kum" , NULL , NULL  , (const char*) l_name_kum } ,
 { "kur" , "ku" , NULL  , (const char*) l_name_kur } ,
 { "kut" , NULL , NULL  , (const char*) l_name_kut } ,
 { "lad" , NULL , NULL  , (const char*) l_name_lad } ,
 { "lah" , NULL , NULL  , (const char*) l_name_lah } ,
 { "lam" , NULL , NULL  , (const char*) l_name_lam } ,
 { "lao" , "lo" , NULL  , (const char*) l_name_lao } ,
 { "lat" , "la" , NULL  , (const char*) l_name_lat } ,
 { "lav" , "lv" , NULL  , (const char*) l_name_lav } ,
 { "lez" , NULL , NULL  , (const char*) l_name_lez } ,
 { "lim" , "li" , NULL  , (const char*) l_name_lim } ,
 { "lin" , "ln" , NULL  , (const char*) l_name_lin } ,
 { "lit" , "lt" , NULL  , (const char*) l_name_lit } ,
 { "lol" , NULL , NULL  , (const char*) l_name_lol } ,
 { "loz" , NULL , NULL  , (const char*) l_name_loz } ,
 { "ltz" , "lb" , NULL  , (const char*) l_name_ltz } ,
 { "lua" , NULL , NULL  , (const char*) l_name_lua } ,
 { "lub" , "lu" , NULL  , (const char*) l_name_lub } ,
 { "lug" , "lg" , NULL  , (const char*) l_name_lug } ,
 { "lui" , NULL , NULL  , (const char*) l_name_lui } ,
 { "lun" , NULL , NULL  , (const char*) l_name_lun } ,
 { "luo" , NULL , NULL  , (const char*) l_name_luo } ,
 { "lus" , NULL , NULL  , (const char*) l_name_lus } ,
 { "mac" , "mk" , "mkd" , (const char*) l_name_mac } ,
 { "mad" , NULL , NULL  , (const char*) l_name_mad } ,
 { "mag" , NULL , NULL  , (const char*) l_name_mag } ,
 { "mah" , "mh" , NULL  , (const char*) l_name_mah } ,
 { "mai" , NULL , NULL  , (const char*) l_name_mai } ,
 { "mak" , NULL , NULL  , (const char*) l_name_mak } ,
 { "mal" , "ml" , NULL  , (const char*) l_name_mal } ,
 { "man" , NULL , NULL  , (const char*) l_name_man } ,
 { "mao" , "mi" , "mri" , (const char*) l_name_mao } ,
 { "map" , NULL , NULL  , (const char*) l_name_map } ,
 { "mar" , "mr" , NULL  , (const char*) l_name_mar } ,
 { "mas" , NULL , NULL  , (const char*) l_name_mas } ,
 { "may" , "ms" , "msa" , (const char*) l_name_may } ,
 { "mdf" , NULL , NULL  , (const char*) l_name_mdf } ,
 { "mdr" , NULL , NULL  , (const char*) l_name_mdr } ,
 { "men" , NULL , NULL  , (const char*) l_name_men } ,
 { "mga" , NULL , NULL  , (const char*) l_name_mga } ,
 { "mic" , NULL , NULL  , (const char*) l_name_mic } ,
 { "min" , NULL , NULL  , (const char*) l_name_min } ,
 { "mis" , NULL , NULL  , (const char*) l_name_mis } ,
 { "mkh" , NULL , NULL  , (const char*) l_name_mkh } ,
 { "mlg" , "mg" , NULL  , (const char*) l_name_mlg } ,
 { "mlt" , "mt" , NULL  , (const char*) l_name_mlt } ,
 { "mnc" , NULL , NULL  , (const char*) l_name_mnc } ,
 { "mni" , NULL , NULL  , (const char*) l_name_mni } ,
 { "mno" , NULL , NULL  , (const char*) l_name_mno } ,
 { "moh" , NULL , NULL  , (const char*) l_name_moh } ,
 { "mon" , "mn" , NULL  , (const char*) l_name_mon } ,
 { "mos" , NULL , NULL  , (const char*) l_name_mos } ,
 { "mul" , NULL , NULL  , (const char*) l_name_mul } ,
 { "mun" , NULL , NULL  , (const char*) l_name_mun } ,
 { "mus" , NULL , NULL  , (const char*) l_name_mus } ,
 { "mwl" , NULL , NULL  , (const char*) l_name_mwl } ,
 { "mwr" , NULL , NULL  , (const char*) l_name_mwr } ,
 { "myn" , NULL , NULL  , (const char*) l_name_myn } ,
 { "myv" , NULL , NULL  , (const char*) l_name_myv } ,
 { "nah" , NULL , NULL  , (const char*) l_name_nah } ,
 { "nai" , NULL , NULL  , (const char*) l_name_nai } ,
 { "nap" , NULL , NULL  , (const char*) l_name_nap } ,
 { "nau" , "na" , NULL  , (const char*) l_name_nau } ,
 { "nav" , "nv" , NULL  , (const char*) l_name_nav } ,
 { "nbl" , "nr" , NULL  , (const char*) l_name_nbl } ,
 { "nde" , "nd" , NULL  , (const char*) l_name_nde } ,
 { "ndo" , "ng" , NULL  , (const char*) l_name_ndo } ,
 { "nds" , NULL , NULL  , (const char*) l_name_nds } ,
 { "nep" , "ne" , NULL  , (const char*) l_name_nep } ,
 { "new" , NULL , NULL  , (const char*) l_name_new } ,
 { "nia" , NULL , NULL  , (const char*) l_name_nia } ,
 { "nic" , NULL , NULL  , (const char*) l_name_nic } ,
 { "niu" , NULL , NULL  , (const char*) l_name_niu } ,
 { "nno" , "nn" , NULL  , (const char*) l_name_nno } ,
 { "nob" , "nb" , NULL  , (const char*) l_name_nob } ,
 { "nog" , NULL , NULL  , (const char*) l_name_nog } ,
 { "non" , NULL , NULL  , (const char*) l_name_non } ,
 { "nor" , "no" , NULL  , (const char*) l_name_nor } ,
 { "nqo" , NULL , NULL  , (const char*) l_name_nqo } ,
 { "nso" , NULL , NULL  , (const char*) l_name_nso } ,
 { "nub" , NULL , NULL  , (const char*) l_name_nub } ,
 { "nwc" , NULL , NULL  , (const char*) l_name_nwc } ,
 { "nya" , "ny" , NULL  , (const char*) l_name_nya } ,
 { "nym" , NULL , NULL  , (const char*) l_name_nym } ,
 { "nyn" , NULL , NULL  , (const char*) l_name_nyn } ,
 { "nyo" , NULL , NULL  , (const char*) l_name_nyo } ,
 { "nzi" , NULL , NULL  , (const char*) l_name_nzi } ,
 { "oci" , "oc" , NULL  , (const char*) l_name_oci } ,
 { "oji" , "oj" , NULL  , (const char*) l_name_oji } ,
 { "ori" , "or" , NULL  , (const char*) l_name_ori } ,
 { "orm" , "om" , NULL  , (const char*) l_name_orm } ,
 { "osa" , NULL , NULL  , (const char*) l_name_osa } ,
 { "oss" , "os" , NULL  , (const char*) l_name_oss } ,
 { "ota" , NULL , NULL  , (const char*) l_name_ota } ,
 { "oto" , NULL , NULL  , (const char*) l_name_oto } ,
 { "paa" , NULL , NULL  , (const char*) l_name_paa } ,
 { "pag" , NULL , NULL  , (const char*) l_name_pag } ,
 { "pal" , NULL , NULL  , (const char*) l_name_pal } ,
 { "pam" , NULL , NULL  , (const char*) l_name_pam } ,
 { "pan" , "pa" , NULL  , (const char*) l_name_pan } ,
 { "pap" , NULL , NULL  , (const char*) l_name_pap } ,
 { "pau" , NULL , NULL  , (const char*) l_name_pau } ,
 { "peo" , NULL , NULL  , (const char*) l_name_peo } ,
 { "per" , "fa" , "fas" , (const char*) l_name_per } ,
 { "phi" , NULL , NULL  , (const char*) l_name_phi } ,
 { "phn" , NULL , NULL  , (const char*) l_name_phn } ,
 { "pli" , "pi" , NULL  , (const char*) l_name_pli } ,
 { "pol" , "pl" , NULL  , (const char*) l_name_pol } ,
 { "pon" , NULL , NULL  , (const char*) l_name_pon } ,
 { "por" , "pt" , NULL  , (const char*) l_name_por } ,
 { "pra" , NULL , NULL  , (const char*) l_name_pra } ,
 { "pro" , NULL , NULL  , (const char*) l_name_pro } ,
 { "pus" , "ps" , NULL  , (const char*) l_name_pus } ,
 { "que" , "qu" , NULL  , (const char*) l_name_que } ,
 { "raj" , NULL , NULL  , (const char*) l_name_raj } ,
 { "rap" , NULL , NULL  , (const char*) l_name_rap } ,
 { "rar" , NULL , NULL  , (const char*) l_name_rar } ,
 { "roa" , NULL , NULL  , (const char*) l_name_roa } ,
 { "roh" , "rm" , NULL  , (const char*) l_name_roh } ,
 { "rom" , NULL , NULL  , (const char*) l_name_rom } ,
 { "rum" , "ro" , "ron" , (const char*) l_name_rum } ,
 { "run" , "rn" , NULL  , (const char*) l_name_run } ,
 { "rup" , NULL , NULL  , (const char*) l_name_rup } ,
 { "rus" , "ru" , NULL  , (const char*) l_name_rus } ,
 { "sad" , NULL , NULL  , (const char*) l_name_sad } ,
 { "sag" , "sg" , NULL  , (const char*) l_name_sag } ,
 { "sah" , NULL , NULL  , (const char*) l_name_sah } ,
 { "sai" , NULL , NULL  , (const char*) l_name_sai } ,
 { "sal" , NULL , NULL  , (const char*) l_name_sal } ,
 { "sam" , NULL , NULL  , (const char*) l_name_sam } ,
 { "san" , "sa" , NULL  , (const char*) l_name_san } ,
 { "sas" , NULL , NULL  , (const char*) l_name_sas } ,
 { "sat" , NULL , NULL  , (const char*) l_name_sat } ,
 { "scn" , NULL , NULL  , (const char*) l_name_scn } ,
 { "sco" , NULL , NULL  , (const char*) l_name_sco } ,
 { "sel" , NULL , NULL  , (const char*) l_name_sel } ,
 { "sem" , NULL , NULL  , (const char*) l_name_sem } ,
 { "sga" , NULL , NULL  , (const char*) l_name_sga } ,
 { "sgn" , NULL , NULL  , (const char*) l_name_sgn } ,
 { "shn" , NULL , NULL  , (const char*) l_name_shn } ,
 { "sid" , NULL , NULL  , (const char*) l_name_sid } ,
 { "sin" , "si" , NULL  , (const char*) l_name_sin } ,
 { "sio" , NULL , NULL  , (const char*) l_name_sio } ,
 { "sit" , NULL , NULL  , (const char*) l_name_sit } ,
 { "sla" , NULL , NULL  , (const char*) l_name_sla } ,
 { "slo" , "sk" , "slk" , (const char*) l_name_slo } ,
 { "slv" , "sl" , NULL  , (const char*) l_name_slv } ,
 { "sma" , NULL , NULL  , (const char*) l_name_sma } ,
 { "sme" , "se" , NULL  , (const char*) l_name_sme } ,
 { "smi" , NULL , NULL  , (const char*) l_name_smi } ,
 { "smj" , NULL , NULL  , (const char*) l_name_smj } ,
 { "smn" , NULL , NULL  , (const char*) l_name_smn } ,
 { "smo" , "sm" , NULL  , (const char*) l_name_smo } ,
 { "sms" , NULL , NULL  , (const char*) l_name_sms } ,
 { "sna" , "sn" , NULL  , (const char*) l_name_sna } ,
 { "snd" , "sd" , NULL  , (const char*) l_name_snd } ,
 { "snk" , NULL , NULL  , (const char*) l_name_snk } ,
 { "sog" , NULL , NULL  , (const char*) l_name_sog } ,
 { "som" , "so" , NULL  , (const char*) l_name_som } ,
 { "son" , NULL , NULL  , (const char*) l_name_son } ,
 { "sot" , "st" , NULL  , (const char*) l_name_sot } ,
 { "spa" , "es" , NULL  , (const char*) l_name_spa } ,
 { "srd" , "sc" , NULL  , (const char*) l_name_srd } ,
 { "srn" , NULL , NULL  , (const char*) l_name_srn } ,
 { "srp" , "sr" , NULL  , (const char*) l_name_srp } ,
 { "srr" , NULL , NULL  , (const char*) l_name_srr } ,
 { "ssa" , NULL , NULL  , (const char*) l_name_ssa } ,
 { "ssw" , "ss" , NULL  , (const char*) l_name_ssw } ,
 { "suk" , NULL , NULL  , (const char*) l_name_suk } ,
 { "sun" , "su" , NULL  , (const char*) l_name_sun } ,
 { "sus" , NULL , NULL  , (const char*) l_name_sus } ,
 { "sux" , NULL , NULL  , (const char*) l_name_sux } ,
 { "swa" , "sw" , NULL  , (const char*) l_name_swa } ,
 { "swe" , "sv" , NULL  , (const char*) l_name_swe } ,
 { "syc" , NULL , NULL  , (const char*) l_name_syc } ,
 { "syr" , NULL , NULL  , (const char*) l_name_syr } ,
 { "tah" , "ty" , NULL  , (const char*) l_name_tah } ,
 { "tai" , NULL , NULL  , (const char*) l_name_tai } ,
 { "tam" , "ta" , NULL  , (const char*) l_name_tam } ,
 { "tat" , "tt" , NULL  , (const char*) l_name_tat } ,
 { "tel" , "te" , NULL  , (const char*) l_name_tel } ,
 { "tem" , NULL , NULL  , (const char*) l_name_tem } ,
 { "ter" , NULL , NULL  , (const char*) l_name_ter } ,
 { "tet" , NULL , NULL  , (const char*) l_name_tet } ,
 { "tgk" , "tg" , NULL  , (const char*) l_name_tgk } ,
 { "tgl" , "tl" , NULL  , (const char*) l_name_tgl } ,
 { "tha" , "th" , NULL  , (const char*) l_name_tha } ,
 { "tib" , "bo" , "bod" , (const char*) l_name_tib } ,
 { "tig" , NULL , NULL  , (const char*) l_name_tig } ,
 { "tir" , "ti" , NULL  , (const char*) l_name_tir } ,
 { "tiv" , NULL , NULL  , (const char*) l_name_tiv } ,
 { "tkl" , NULL , NULL  , (const char*) l_name_tkl } ,
 { "tlh" , NULL , NULL  , (const char*) l_name_tlh } ,
 { "tli" , NULL , NULL  , (const char*) l_name_tli } ,
 { "tmh" , NULL , NULL  , (const char*) l_name_tmh } ,
 { "tog" , NULL , NULL  , (const char*) l_name_tog } ,
 { "ton" , "to" , NULL  , (const char*) l_name_ton } ,
 { "tpi" , NULL , NULL  , (const char*) l_name_tpi } ,
 { "tsi" , NULL , NULL  , (const char*) l_name_tsi } ,
 { "tsn" , "tn" , NULL  , (const char*) l_name_tsn } ,
 { "tso" , "ts" , NULL  , (const char*) l_name_tso } ,
 { "tuk" , "tk" , NULL  , (const char*) l_name_tuk } ,
 { "tum" , NULL , NULL  , (const char*) l_name_tum } ,
 { "tup" , NULL , NULL  , (const char*) l_name_tup } ,
 { "tur" , "tr" , NULL  , (const char*) l_name_tur } ,
 { "tut" , NULL , NULL  , (const char*) l_name_tut } ,
 { "tvl" , NULL , NULL  , (const char*) l_name_tvl } ,
 { "twi" , "tw" , NULL  , (const char*) l_name_twi } ,
 { "tyv" , NULL , NULL  , (const char*) l_name_tyv } ,
 { "udm" , NULL , NULL  , (const char*) l_name_udm } ,
 { "uga" , NULL , NULL  , (const char*) l_name_uga } ,
 { "uig" , "ug" , NULL  , (const char*) l_name_uig } ,
 { "ukr" , "uk" , NULL  , (const char*) l_name_ukr } ,
 { "umb" , NULL , NULL  , (const char*) l_name_umb } ,
 { "und" , NULL , NULL  , (const char*) l_name_und } ,
 { "urd" , "ur" , NULL  , (const char*) l_name_urd } ,
 { "uzb" , "uz" , NULL  , (const char*) l_name_uzb } ,
 { "vai" , NULL , NULL  , (const char*) l_name_vai } ,
 { "ven" , "ve" , NULL  , (const char*) l_name_ven } ,
 { "vie" , "vi" , NULL  , (const char*) l_name_vie } ,
 { "vol" , "vo" , NULL  , (const char*) l_name_vol } ,
 { "vot" , NULL , NULL  , (const char*) l_name_vot } ,
 { "wak" , NULL , NULL  , (const char*) l_name_wak } ,
 { "wal" , NULL , NULL  , (const char*) l_name_wal } ,
 { "war" , NULL , NULL  , (const char*) l_name_war } ,
 { "was" , NULL , NULL  , (const char*) l_name_was } ,
 { "wel" , "cy" , "cym" , (const char*) l_name_wel } ,
 { "wen" , NULL , NULL  , (const char*) l_name_wen } ,
 { "wln" , "wa" , NULL  , (const char*) l_name_wln } ,
 { "wol" , "wo" , NULL  , (const char*) l_name_wol } ,
 { "xal" , NULL , NULL  , (const char*) l_name_xal } ,
 { "xho" , "xh" , NULL  , (const char*) l_name_xho } ,
 { "yao" , NULL , NULL  , (const char*) l_name_yao } ,
 { "yap" , NULL , NULL  , (const char*) l_name_yap } ,
 { "yid" , "yi" , NULL  , (const char*) l_name_yid } ,
 { "yor" , "yo" , NULL  , (const char*) l_name_yor } ,
 { "ypk" , NULL , NULL  , (const char*) l_name_ypk } ,
 { "zap" , NULL , NULL  , (const char*) l_name_zap } ,
 { "zbl" , NULL , NULL  , (const char*) l_name_zbl } ,
 { "zen" , NULL , NULL  , (const char*) l_name_zen } ,
 { "zha" , "za" , NULL  , (const char*) l_name_zha } ,
 { "znd" , NULL , NULL  , (const char*) l_name_znd } ,
 { "zul" , "zu" , NULL  , (const char*) l_name_zul } ,
 { "zun" , NULL , NULL  , (const char*) l_name_zun } ,
 { "zxx" , NULL , NULL  , (const char*) l_name_zxx } ,
 { "zza" , NULL , NULL  , (const char*) l_name_zza } ,
};

