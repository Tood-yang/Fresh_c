#include "sdk.h"
#include <math.h>
#include <cstdlib>
extern"C" __declspec(dllexport) void ai(my_info ,Map ,Action&);
extern"C" __declspec(dllexport) int choose_race();
//全局变量定义区
my_info history_info,temp_info;

//多回合数据
//static int once_hp=300;		//储存上一回合血量，判断是否受到攻击
//自定义函数声明区
point att_unit(Unit enemy_units[],Hero hero,int num);		//预定攻击目标坐标
Unit nst_my_unit();		//最近我方单位
Unit nst_enemy_unit(Unit enemy[],Hero hero,int num);	//最近敌方单位
int in_tower(Tower enemy[],Hero hero,int num);		//是否在敌方塔中，是1否0
Tower nst_tower(Tower enemy[],Hero hero,int num);	//最近敌方塔
int dying_unit(Unit enemy_units[],Hero hero,int num);	//判断是否有即将死亡的敌方单位
int hero_cmp();
Unit yeguai();

int choose_race(){  //选择种族，0代表暗夜精灵（NE），1代表兽族（ORC）
	return 0;
}

void ai(my_info info,Map map,Action& cmd){
	static int ending=0,dayeguai=1,yeguai_hp=2000,yeguai_alive=1;
	int i,js=0,m;
	Hero my_hero=info.my_hero;  //获取我方英雄
	//跨回合信息更新****************************************************
	history_info=temp_info;
	temp_info=info;
	if(dying_unit(info.enemy_units,my_hero,info.num_enemy_units)==1 && info.my_hero.level<20)  //有能补刀直接补
	{
		cmd.action=Action::ATTACK;
		cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);return;
	}
	//前期策略**********************************************************
	if (my_hero.level<=10)
	{
		//技能策略******************************************************
		if(my_hero.qui.cd_next==0 && my_hero.qui.mag_cos<=my_hero.mag)	//技能可释放时
		{
			if (my_hero.hp<my_hero.Hp-80)
			{
				cmd.action=Action::MAGIC;return;//英雄血量低于上限-150直接回血
			}
			for(m=0;m<info.num_my_units;m++)		//计数施法范围内受伤单位数
			{
				if(info.my_units[m].hp>info.my_units[m].Hp-80)		//判断友方单位是否受伤
					continue;
				if(in_range(my_hero.pos,info.my_units[m].pos,6.0)==1)
				{
					js++;		//单位轻伤
					if(info.my_units[m].hp<=info.my_units[m].Hp-150)
						js++;		//单位重伤，计数再加一
				}
			}
			if ((js>=2 /*&& my_hero.hp<=my_hero.Hp-40*/) || (my_hero.hp<=my_hero.Hp-40 && js>=1))		//判断是否满足施法条件(计数大于2，或英雄受轻伤、计数大于1)
			{
				cmd.action=Action::MAGIC;return;
			}
		}
		//走位、攻击策略******************************************************
		//若有敌方单位近身，是，若不在塔下，则后撤（1,1），在塔下则进攻，否，下行
		//攻击范围内有无敌方单位，是，攻击，否，下行
		//判断敌方英雄是否在攻击范围内，是，判断强弱，强打弱退。否，站中点
		//if(dying_unit(info.enemy_units,my_hero,info.num_enemy_units)==1)  //有能补刀直接补
		//{
		//	cmd.action=Action::ATTACK;
		//	cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);return;
		//}
		if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,3.0))		//敌方单位近身
		{
			if(in_range(my_hero.pos,my_hero.id==0?point(45,13):point(63,31),3.0))	//在塔下
			{
				cmd.action=Action::ATTACK;
				cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
				return;
			}
			else	//不在塔下
			{
				cmd.action=Action::MOVE;
				cmd.pos=my_hero.id==0?point(45,13):point(63,31);
				return;
			}
		}		//敌方单位未近身
		if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//攻击范围中有敌方单位
		{
			cmd.action=Action::ATTACK;
			cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
			return;
		}
		if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0)==0)	//攻击范围中没有敌方单位
		{
			if(info.enemy_hero_vsb==true && in_range(my_hero.pos,info.enemy_hero.pos,6.0)==1)		//敌方英雄在视野内
			{
				if (hero_cmp()==1)		//我方无大劣势
					{
						if (in_range(my_hero.pos,info.enemy_hero.pos,5.0)==1)
						{
						cmd.action=Action::ATTACK;
						cmd.pos=info.enemy_hero.pos;return;		//攻击对方英雄
						}
						else
						{
							cmd.action=Action::MOVE;
							cmd.pos=info.enemy_hero.pos;
							return;
						}
					}
				else
				{
					cmd.action=Action::MOVE;
					cmd.pos=my_hero.id==0?point(45,13):point(63,31);
					return;		//敌方强，退回己方下塔下
				}
			}
			else		//攻击范围内啥也没有
			{
				cmd.action=Action::MOVE;
				cmd.pos=my_hero.id==0?point (58,26):point(50,18);
				return;
			}
		}
	}
	//中期
	if(my_hero.level<=14)
	{
		if(my_hero.qui.cd_next==0 && my_hero.qui.mag_cos<=my_hero.mag)	//技能可释放时
		{
			if (my_hero.hp<my_hero.Hp-100)
			{
				cmd.action=Action::MAGIC;return;//英雄血量低于上限-120直接回血
			}
			for(m=0;m<info.num_my_units;m++)		//计数施法范围内受伤单位数
			{
				if(info.my_units[m].hp>info.my_units[m].Hp-180)		//判断友方单位是否受伤
					continue;
				if(in_range(my_hero.pos,info.my_units[m].pos,6.0)==1)
				{
					js++;		//单位轻伤
					if(info.my_units[m].hp<=info.my_units[m].Hp-250)
						js++;		//单位重伤，计数再加一
				}
			}
			if ((js>=2 /*&& my_hero.hp<=my_hero.Hp-80*/) || (my_hero.hp<=my_hero.Hp-70 && js>=1))		//判断是否满足施法条件(计数大于2，或英雄受轻伤、计数大于1)
			{
				cmd.action=Action::MAGIC;return;
			}
		}
		//攻击策略
		if (in_range(my_hero.pos,point(73,3),8.0)==0)
		{
			if (/*my_hero.hp/(60-my_hero.def)>yeguai_hp/(my_hero.atk-20) &&*/ yeguai_alive==1  && \
				in_range(nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,my_hero.pos,5.0)==0 && my_hero.hp>=my_hero.Hp*0.8)
				dayeguai=1;
		}
		if (dayeguai==0)
		{
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,3.0))		//敌方单位近身
			{
				if(in_range(my_hero.pos,my_hero.id==0?point(45,13):point(63,31),3.0))	//在塔下
				{
					cmd.action=Action::ATTACK;
					cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
					return;
				}
				else	//不在塔下
				{
					cmd.action=Action::MOVE;
					cmd.pos=my_hero.id==0?point(45,13):point(63,31);
					return;
				}
			}		//敌方单位未近身
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//攻击范围中有敌方单位
			{
				cmd.action=Action::ATTACK;
				cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
				return;
			}
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0)==0)	//攻击范围中没有敌方单位
			{
				if(info.enemy_hero_vsb==true && in_range(my_hero.pos,info.enemy_hero.pos,6.0)==1)		//敌方英雄在视野内
				{
					if (hero_cmp()==1)		//我方无大劣势
					{
						if (in_range(my_hero.pos,info.enemy_hero.pos,5.0)==1)
						{
						cmd.action=Action::ATTACK;
						cmd.pos=info.enemy_hero.pos;return;		//攻击对方英雄
						}
						else
						{
							cmd.action=Action::MOVE;
							cmd.pos=info.enemy_hero.pos;
							return;
						}
					}
					else
					{
						cmd.action=Action::MOVE;
						cmd.pos=my_hero.id==0?point(45,13):point(63,31);
						return;		//敌方强，退回己方下塔下
					}
				}
				else		//攻击范围内啥也没有
				{
					cmd.action=Action::MOVE;
					cmd.pos=my_hero.id==0?point (58,26):point(50,18);
					return;
				}
			}
		}
		if (dayeguai==1)
		{
			if (yeguai().id==-1)
			{
				yeguai_hp=yeguai().hp;
			}
			if ((my_hero.hp<=200 && info.enemy_hero_vsb==1 && in_range(my_hero.pos,info.enemy_hero.pos,5)) || yeguai_alive==0)			//如果被打死了，改血量
			{
				dayeguai=0;
				cmd.action=Action::MOVE;
				cmd.pos=my_hero.id==0?point(45,13):point(63,31);
				return;
			}
			if (in_range(my_hero.pos,point(73,3),3)==1)
			{
				int judge=0;
				for (int t=0;t<info.num_enemy_units;t++)
				{
					if (info.enemy_units[t].id==-1)
					{
						judge=1;
						break;
					}
				}
				yeguai_alive=judge;
			}
			if (info.enemy_hero_vsb==0 || in_range(my_hero.pos,info.enemy_hero.pos,5.0)==0)
			{
				if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//攻击范围中有敌方单位
				{
					cmd.action=Action::ATTACK;
					cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
					return;
				}
				else
				{
					cmd.action=Action::MOVE;
					cmd.pos=point(73,3);
					return;
				}
			}
			if (in_range(my_hero.pos,info.enemy_hero.pos,5.0)==1)
			{
				if (yeguai().pos != my_hero.pos && in_range(yeguai().pos,my_hero.pos,5))
				{
					if (yeguai().atkpos == info.enemy_hero.pos)
					{
						if (info.enemy_hero.hp <= (my_hero.atk + yeguai().atk - info.enemy_hero.def))
						{
							cmd.action=Action::ATTACK;
							cmd.pos=info.enemy_hero.pos;
							return;
						}
						else
						{
							cmd.action=Action::ATTACK;
							cmd.pos=yeguai().pos;
							return;
						}
					}
					else
					{
						if (info.enemy_hero.hp <=(my_hero.atk - info.enemy_hero.def))
						{
							cmd.action=Action::ATTACK;
							cmd.pos=info.enemy_hero.pos;
							return;
						}
						else
						{
							cmd.action=Action::ATTACK;
							cmd.pos=yeguai().pos;
							return;
						}
					}
				}
				else
				{
					cmd.action=Action::ATTACK;
					cmd.pos=info.enemy_hero.pos;
					return;
				}
				/*if (yeguai().id==-1 && my_hero.hp/((history_info.my_hero.hp-my_hero.hp)<=0?1:(history_info.my_hero.hp-my_hero.hp))\
				>(yeguai().hp/(my_hero.atk-yeguai().def)))
				{
					cmd.action==Action::ATTACK;
					cmd.pos=yeguai().pos;
					return;
				}
				if (yeguai().id==-1 && yeguai().atkpos==my_hero.pos)
				{
					cmd.action=Action::MOVE;
					cmd.pos=my_hero.id==0?point(45,13):point(63,31);
					return;
				}
				if (in_range(my_hero.pos,info.enemy_hero.pos,5.0)==1)
				{
					cmd.action=Action::ATTACK;
					cmd.pos=info.enemy_hero.pos;
					return;
				}
				else
				{
					cmd.action=Action::MOVE;
					cmd.pos=info.enemy_hero.pos;
					return;
				}*/
			}
			/*if (hero_cmp()==1)
			{
			cmd.action=Action::ATTACK;
			cmd.pos=info.enemy_hero.pos;
			return;
			}
			if (yeguai().id!=-1)
			{
			cmd.action=Action::ATTACK;
			cmd.pos=info.enemy_hero.pos;
			return;
			}
			if (yeguai().id==-1 && yeguai().atkpos!=my_hero.pos)
			{
			cmd.action=Action::ATTACK;
			cmd.pos=info.enemy_hero.pos;
			return;
			}
			if (yeguai().id==-1 && yeguai().atkpos==my_hero.pos)
			{

			}*/
		}
	}
	else
	{
		if(my_hero.qui.cd_next==0 && my_hero.qui.mag_cos<=my_hero.mag)	//技能可释放时
		{
			if (my_hero.hp<my_hero.Hp-100)
			{
				cmd.action=Action::MAGIC;return;//英雄血量低于上限-120直接回血
			}
			for(m=0;m<info.num_my_units;m++)		//计数施法范围内受伤单位数
			{
				if(info.my_units[m].hp>info.my_units[m].Hp-180)		//判断友方单位是否受伤
					continue;
				if(in_range(my_hero.pos,info.my_units[m].pos,6.0)==1)
				{
					js++;		//单位轻伤
					if(info.my_units[m].hp<=info.my_units[m].Hp-250)
						js++;		//单位重伤，计数再加一
				}
			}
			if ((js>=2 /*&& my_hero.hp<=my_hero.Hp-80*/) || (my_hero.hp<=my_hero.Hp-70 && js>=1))		//判断是否满足施法条件(计数大于2，或英雄受轻伤、计数大于1)
			{
				cmd.action=Action::MAGIC;return;
			}
		}
		int pos_total;
		pos_total = my_hero.pos.xp + my_hero.pos.yp;
		if (my_hero.id==0 ? (pos_total<93) : (pos_total>57))
		{
			cmd.action=Action::MOVE;
			cmd.pos=my_hero.id==0?point(73,20):point(55,2);
			return;
		}
		else
		{
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//攻击范围中有敌方单位
			{
				cmd.action=Action::ATTACK;
				cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
				return;
			}
			if(in_range(nst_tower(info.enemy_towers,my_hero,info.num_enemy_towers).pos,my_hero.pos,5.0)==1)		//敌塔可以攻击
			{
				cmd.action=Action::ATTACK;
				cmd.pos=nst_tower(info.enemy_towers,my_hero,info.num_enemy_towers).pos;
				return;
			}
			if(info.enemy_base_vsb==true && in_range(my_hero.pos,info.enemy_base.pos,my_hero.rng))		//有基地，打基地
			{
				cmd.action=Action::ATTACK;
				cmd.pos=info.enemy_base.pos;return;
			}
			else 
				cmd.action=Action::MOVE;
			cmd.pos=my_hero.id==0?point(74,74):point(1,1);
			return;
		}
	}
}

//自定义函数定义区
Unit nst_enemy_unit(Unit enemy[],Hero hero,int num)		//最近敌方单位
{
	int t=0,z=0;
	double dcur,dmin=200;
	for (t=0;t<num;t++)
	{
		dcur=dis(hero.pos,enemy[t].pos);
		if (dcur<dmin)
		{
			dmin=dcur;
			z=t;
		}
	}
	return (enemy[z]);
}

point att_unit(Unit enemy[],Hero my_hero,int e_num)		//预定攻击单位（暂定最近单位）
{
	//return(nst_enemy_unit(enemy,my_hero,e_num)).pos;
	int min_hp=10000;
	if(temp_info.enemy_hero_vsb && in_range(my_hero.pos,temp_info.enemy_hero.pos,my_hero.rng) 
		&& temp_info.enemy_hero.hp <= my_hero.atk-temp_info.enemy_hero.def)
		return temp_info.enemy_hero.pos;
	for(int i=0;i<temp_info.num_enemy_towers;i++)
	{
		if(temp_info.enemy_towers[i].vsb && in_range(temp_info.enemy_towers[i].pos,my_hero.pos,5.0)==1 
			&& temp_info.enemy_towers[i].hp<=my_hero.atk-temp_info.enemy_towers[i].def)
			return temp_info.enemy_towers[i].pos;
	}
	for(int i=0;i<temp_info.num_my_towers;i++)
	{
		if(in_range(temp_info.my_towers[i].pos,my_hero.pos,5.0)==1 
			&& temp_info.my_towers[i].hp<=my_hero.atk-temp_info.my_towers[i].def
			&& in_range(temp_info.enemy_hero.pos,temp_info.my_towers[i].pos,6.0))
			return temp_info.my_towers[i].pos;
	}

	for (int t=0;t<temp_info.num_enemy_units;t++)
	{
		if (temp_info.enemy_units[t].id==-1)
			return temp_info.enemy_units[t].pos;
	}

	int rnd=10000,fst_num=-1;
	for (int type=2;type>=0;type--)
	{
		for(int i=0;i<e_num;i++)
		{
			if (in_range(my_hero.pos,enemy[i].pos,5.0)==0)
				continue;
			if (enemy[i].status==type && enemy[i].hp <= (my_hero.atk-enemy[i].def))
				return enemy[i].pos;
		}
		for(int i=0;i<e_num;i++)
		{
			if (enemy[i].status==type)
			{
				if (in_range(my_hero.pos,enemy[i].pos,5.0)==0)
					continue;
				int j=0;
				for (j=0;j<history_info.num_enemy_units;j++)
				{
					if (history_info.enemy_units[j].number == temp_info.enemy_units[i].number)
						break;
				}
				if (j==history_info.num_enemy_units)
					continue;
				if (temp_info.enemy_units[i].hp<=min_hp)
				{
					fst_num=i;
				}
			}
		}
		for (int i=0;i<temp_info.num_my_units;i++)
		{
			if (in_range(my_hero.pos,temp_info.my_units[i].pos,5.0) 
				&& temp_info.my_units[i].status==type 
				&& temp_info.my_units[i].hp <= (my_hero.atk-temp_info.my_units[i].def)
				&& temp_info.enemy_hero_vsb
				&& in_range(temp_info.enemy_hero.pos,temp_info.my_units[i].pos,6.0))
				return temp_info.my_units[i].pos;
		}
	}
	if (fst_num==-1) return(nst_enemy_unit(enemy,my_hero,e_num).pos);
	else return enemy[fst_num].pos;
}

int in_tower(Tower enemy[],Hero hero,int num)
{
	int f=0;
	for(f=0;f<num;f++)
	{
		if (in_range(hero.pos,enemy[f].pos,6.0)==1)
			return 1;
	}
	return 0;
}

Tower nst_tower(Tower enemy[],Hero hero,int num)
{
	int f=0,z=0;
	double dcur,dmin=200;
	for(f=0;f<num;f++)
	{
		if(dis(hero.pos,enemy[f].pos)<=dmin)
		{
			dmin=dis(hero.pos,enemy[f].pos);
			z=f;
		}
	}
	return (enemy[z]);
}

int dying_unit(Unit enemy[],Hero my_hero,int e_num)
{
	for (int type=2;type>=0;type--)
	{
		for(int i=0;i<e_num;i++)
		{
			if (in_range(my_hero.pos,enemy[i].pos,5.0)==0)
				continue;
			if (enemy[i].status==type && enemy[i].hp <= (my_hero.atk-enemy[i].def))
				return 1;
		}
	}
	for (int type=2;type>=0;type--)
	{
		for(int i=0;i<temp_info.num_my_units;i++)
		{
			if (in_range(my_hero.pos,temp_info.my_units[i].pos,5.0)==0)
				continue;
			if (temp_info.my_units[i].status==type && temp_info.my_units[i].hp <= (my_hero.atk-temp_info.my_units[i].def))
				return 1;
		}
	}
	return 0;
}

int hero_cmp()
{
	if (history_info.enemy_hero_vsb==0 || history_info.enemy_hero.hp==temp_info.enemy_hero.hp || history_info.my_hero.hp==temp_info.my_hero.hp)
		return 1;		//上回合未见对方英雄，或上回合英雄没有掉血，或者我没有掉血，先打一下
	else
	{
		if(temp_info.my_hero.hp/(history_info.my_hero.hp-temp_info.my_hero.hp)>=
			temp_info.enemy_hero.hp/((history_info.enemy_hero.hp-temp_info.enemy_hero.hp)<=0?\
			(temp_info.my_hero.atk - temp_info.enemy_hero.def):(history_info.enemy_hero.hp-temp_info.enemy_hero.hp)))
			return 1;		//我方英雄占优
		else
			return 0;		//对方英雄占优
	}
}

Unit yeguai()
{
	for (int t=0;t<temp_info.num_enemy_units;t++)
	{
		if (temp_info.enemy_units[t].number==-1)
			return temp_info.enemy_units[t];
	}
	return temp_info.my_hero;
}