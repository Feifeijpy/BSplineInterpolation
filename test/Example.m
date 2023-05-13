%% compile

% mex COMPFLAGS='$COMPFLAGS /std:c++20' bspline.cpp
mex COMPFLAGS='$COMPFLAGS /std:c++20' ...
    '-I..\src\include\BSplineInterpolation' bspline.cpp

%% 1D

% coor_1d = [0.905515, 0.894638, -0.433134, 0.43131, -0.131052, 0.262974, 0.423888, -0.562671, -0.915567, -0.261017, -0.47915, -0.00939326, -0.445962];
% range = [0,6];
% isperiodic = false;
% derivative = uint64(0);


%% 1D periodic

order = uint64(4);
isperiodic = true;
range = [0,12];
derivative = uint64(0);
f1d = [0.905515,0.894638,-0.433134,0.43131,-0.131052,0.262974,0.423888,-0.562671,-0.915567,-0.261017,-0.47915,-0.00939326,-0.445962]';
coor1d = [3.937582749415922,0.46794224590095723,8.366325011606818,11.488876902601298,0.645634679848758,10.863461372085801,2.5491368777592243,0.4267839509148281,3.064288819169027,1.28330941106692]';

result1d = bspline(order,isperiodic,range,f1d(1:end-1),coor1d,derivative);

result1d_mma = [-0.09762254647017743,1.168800757853312,-0.6682906902062101,0.44128062499073417,1.1642814012848224,-0.12481864202139212,-0.00445415461594469,1.159234754035029,0.4508845136779133,0.45967108162968584]';
abs_error = mean(abs(result1d_mma-result1d))

%% 1D periodic derivative
order = uint64(8);
isperiodic = true;
range = [0,2*pi];
derivative = uint64(1);
n = 24; n2 = 100;
x = (0:n)'/n*2*pi;
f1d = sin(x);
x1d = (0:n2)'/n2*2*pi;
result1d = bspline(order,isperiodic,range,f1d(1:end-1),x1d,derivative);
ref_1d = cos(x1d);

figure
hold on
plot(x1d,result1d)
plot(x1d,ref_1d)
abs_error = mean(abs(result1d-ref_1d))

%% 2D

order = uint64(3);
isperiodic = [false,false]';
range = [0,4;0,4];
derivative = uint64([0,0])';
f2d_1d = [0.1149376591802711,0.366986491879822,0.8706293477610783,0.35545268902617266,0.5618622985788111,0.20103081669915168,-0.35136997700640915,0.3459158000280991,-0.6411854570589961,-0.924275445068762,0.0609591391461084,-0.9863975752412686,0.6961713590119816,0.8400181479240318,0.3758703506622121,-0.849674573235994,0.7037626673100483,0.5167482535099701,-0.961602882417603,0.9016008505150341,-0.7763245335851283,0.729592963642911,-0.5861424925445622,-0.3508132480272552,0.7075736670162986];
f2d = reshape(f2d_1d,5,5);
coor2d_1d = [2.7956403386992656,0.16702594760696154,0.14620918612600242,1.80574833501798,0.912690726300375,1.617752882497217,3.4743760585635837,3.235474073888084,3.2819951814307826,1.2227470066832273,2.6455842429367955,2.5079261616463135,2.4398083528195187,2.6353068861468163,0.3906420637429102,0.4383471604184015,3.702137078579508,3.366239031714958,1.0831817822035914,1.7656688800854026];
coor2d = reshape(coor2d_1d,2,[])';
coor2d2 = fliplr(coor2d);

result2d = bspline(order,isperiodic,range,f2d,coor2d2,derivative);

result2d_mma=[-0.5407817111102959,0.724190993527158,0.25189765520792373,-1.4791573194664718,1.1465605774177101,0.3098484329528253,0.5464869245836507,0.0474466547034479,-1.0511757647770876,0.2755974099701995]';
abs_error = mean(abs(result2d_mma-result2d))


%% 2D periodic
isperiodic = [true, false]';
result2d = bspline(order,isperiodic,range,f2d(1:end-1,:),coor2d2,derivative);
result2d_mma = [-0.5456439415470818,0.7261218483070795,0.21577722210958022,-1.6499933881987376,1.224021619908732,0.34969937176489274,0.5845798304532657,0.2875923130734858,-1.4740569960870218,0.258215214830246]';
% 
abs_error = mean(abs(result2d_mma-result2d))

%% 3D

order = uint64(3);
isperiodic = [false,false,false]';
range = [0,6;0,5;0,4];
derivative = uint64([0,0,0])';
f3d_1d = [0.7227454556577735, 0.12338574902709754, 0.3377337587737288,0.8091623938771417, 0.9293380971320708, -0.5741047532957699,            -0.2200926742338356,           -0.10763006732480607, 0.0013429236738118355, 0.4513036540324049,            -0.026003741605878705, -0.3695306079209315, 0.8136434953103922,            -0.5116150484018025,           -0.8677823131366349, 0.8365100339234104, 0.7501947382164902,            0.29910348046032365, 0.6232336678925097, 0.5087192462972618,            -0.893065394522043,           -0.4851529104440644, 0.03957209074264867, 0.2539529968314116,            -0.34512413139965936, -0.15618833950707955, -0.41941677707757874,            0.4458983469783453,           -0.7660093051011847, 0.5482763224961875, -0.9987913478497763,            -0.9842994610123474, -0.7801902911749243, 0.5613574074106569,            -0.49371188067385186,           0.5583049111784795, -0.9511960118489133, -0.7610577450906089,            0.48188343529691346, 0.23547454702291803, 0.20415133442747058,            0.9181145422295938,         0.14911700752757273, -0.05837746001531219, 0.5368187139585459,            -0.8940613010669436, -0.5018302442340068, 0.27020723614181774,            0.12359775508403592,           -0.3683916194955037, -0.5525267297116154, -0.5872030595156685,            -0.8061235674442466, -0.23709048741408623, 0.42772609271483164,            0.4827241146251704,           -0.8997166066727429, 0.09739710605428176, 0.5320698398177925,            0.05469688654269378, 0.4503706682941915, -0.7821018114726392,            -0.3379863042117526,           0.5105732505948533, 0.6195932582442767, 0.6841027629496503,            -0.024765852986073256, 0.10834864531079891, -0.49612833775897114,            0.6109637757093997,           -0.28292428408539205, 0.30657791183926, -0.4741374456208911,            0.714641208996071, 0.8309281186389432, 0.20447906224501944,            0.1498769698488771,           0.38814726304764857, -0.43209235857228956, -0.8375165379497882,            -0.9320039920129055, -0.5820061765266624, -0.6009461842921451,            0.8425964963548309,         0.6848105040087451, -0.09424447387569135, 0.630497576004454,            0.319121226100175, 0.15547601325199478, 0.37766869124805513,            -0.418251240744814,           -0.5841177409538716, -0.07943995214433475, 0.6405419622330082,            -0.18822915511374738, 0.9682166115359014, -0.24310913205955398,            0.4207346330747752,           0.45689131291795304, 0.5592009847191926, -0.5794736694118892,            0.2777598519942086, 0.06893779977151926, -0.10558235108004288,            0.24127228408049373,           0.8653133697361124, 0.8808125543307121, 0.013003929742553488,            -0.25819110436923287, 0.7150667606571139, -0.8474607538945635,            -0.21182942625066659,           0.7953892915393812, -0.5146297490600227, 0.9797805357099336,            -0.19418087953367502, 0.30615862986502584, 0.9621224147788383,            0.591540018663443,           -0.0326501323196986, 0.4340496651711856, -0.6274862226468154,            0.43246185178053453, -0.6752962418803996, -0.11639979303524317,            0.04992688161509218,         0.7246870093148794, 0.41031155832285204, 0.6075964984936815,            -0.9309929613876653, -0.5305318949868476, -0.684400029780897,            -0.03189572751676284,           0.3890456863699665, 0.055814217385724785, -0.028984551458518748,            0.1946456227803317, -0.28468283193227384, 0.07443098789465097,            -0.3397710281207207,           -0.3252622666425089, -0.7764904739232343, 0.659017163729533,            -0.6314623347234112, -0.4459102255849552, -0.8305995999617672,            -0.6736542039955138,           -0.09946179372459873, 0.48571832213388744, 0.06431964245524391,            0.9248711036318769, 0.27818775843144383, 0.06436195186180793,            0.4804631389346339,           0.8394854989160261, 0.46911286378594497, -0.3880347646613327,            -0.8793296857106343, 0.7535849141300499, -0.14621751679049622,            -0.24084757493862208,           0.263291050906274, 0.2426439223615553, 0.024235715636066857,            -0.3441033743486446, -0.6157381917061411, 0.8654487680234961,            -0.015712235651818673,         0.25941803193881485, -0.5643528433065192, -0.6939218246816452,            -0.6573164675211882, 0.3044833933508735, -0.15696192470423354,            -0.7292088799733678,           0.7157059796941971, 0.5010086085806371, 0.42635964783799274,            -0.6918122089292549, 0.5343027744642965, -0.3177068701933763,            0.7728881262187897,           0.1919928613372428, 0.9481231231381191, -0.8495829859085204,            -0.5016908143642169, -0.25281765568651915, -0.8041546515214235,            -0.9379455299920623,           0.9756710429966389, 0.002338101461675013, 0.5512942665704528,            0.11255265169286277, -0.4446511594248159, 0.923624877032104,            -0.35888035429907994,           -0.4993622134330429, -0.41713411728302896, 0.5241211722633352,            -0.8565133189111758, 0.009666595018221535, 0.0024308669289925255,            0.21168620320785259,           0.7819003932609805, 0.9688407967537689, -0.438602023010886,            0.6460148816650522, -0.463700457054959, 0.7497559992824492,            -0.6604977204100679];
f3d = reshape(f3d_1d,7,6,5);
coor3d_1d = [3.702442458842895, 0.3823502932775078, 2.0413783528852125,0.5158748832148454, 3.998837357745451, 3.063577604910418,         0.9488026738796771, 1.0559217201840418, 2.5167646532589334,         3.787943594036949, 1.0660346792124562, 0.39476961351333895,         2.5437164557222864, 2.1812414553861963, 3.194304608069536,         0.8943435648540952, 3.274226260967546, 0.3988216219352534,         2.607886328617485, 1.718155000146739, 3.5736295561232927,         3.0725177261490373, 1.054538982002727, 0.4944889286172529,         1.7731799360168479, 1.237539640884, 1.0101529018305797,         0.8655632442457968, 1.3379555874162659, 1.7103327524709568];
coor3d = reshape(coor3d_1d,3,[])';
coor3d2 = fliplr(coor3d);

result3d = bspline(order,isperiodic,range,f3d,coor3d2,derivative);

result3d_mma = [-0.20015704400375117,0.5183267778903129,-0.7197026899005371,0.5183443130595233,-0.07245353025037997,0.5534353986280456,-0.2674229002109916,0.1673843822053797,-0.021928200124974297,-0.260677062462001]';
abs_error = mean(abs(result3d_mma-result3d))
