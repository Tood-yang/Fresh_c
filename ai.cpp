#include "sdk.h"
#include <math.h>
#include <cstdlib>
extern"C" __declspec(dllexport) void ai(my_info ,Map ,Action&);
extern"C" __declspec(dllexport) int choose_race();
//ȫ�ֱ���������
my_info history_info,temp_info;

//��غ�����
//static int once_hp=300;		//������һ�غ�Ѫ�����ж��Ƿ��ܵ�����
//�Զ��庯��������
point att_unit(Unit enemy_units[],Hero hero,int num);		//Ԥ������Ŀ������
Unit nst_my_unit();		//����ҷ���λ
Unit nst_enemy_unit(Unit enemy[],Hero hero,int num);	//����з���λ
int in_tower(Tower enemy[],Hero hero,int num);		//�Ƿ��ڵз����У���1��0
Tower nst_tower(Tower enemy[],Hero hero,int num);	//����з���
int dying_unit(Unit enemy_units[],Hero hero,int num);	//�ж��Ƿ��м��������ĵз���λ
int hero_cmp();
Unit yeguai();

int choose_race(){  //ѡ�����壬0����ҹ���飨NE����1�������壨ORC��
	return 0;
}

void ai(my_info info,Map map,Action& cmd){
	static int ending=0,dayeguai=1,yeguai_hp=2000,yeguai_alive=1;
	int i,js=0,m;
	Hero my_hero=info.my_hero;  //��ȡ�ҷ�Ӣ��
	//��غ���Ϣ����****************************************************
	history_info=temp_info;
	temp_info=info;
	if(dying_unit(info.enemy_units,my_hero,info.num_enemy_units)==1 && info.my_hero.level<20)  //���ܲ���ֱ�Ӳ�
	{
		cmd.action=Action::ATTACK;
		cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);return;
	}
	//ǰ�ڲ���**********************************************************
	if (my_hero.level<=10)
	{
		//���ܲ���******************************************************
		if(my_hero.qui.cd_next==0 && my_hero.qui.mag_cos<=my_hero.mag)	//���ܿ��ͷ�ʱ
		{
			if (my_hero.hp<my_hero.Hp-80)
			{
				cmd.action=Action::MAGIC;return;//Ӣ��Ѫ����������-150ֱ�ӻ�Ѫ
			}
			for(m=0;m<info.num_my_units;m++)		//����ʩ����Χ�����˵�λ��
			{
				if(info.my_units[m].hp>info.my_units[m].Hp-80)		//�ж��ѷ���λ�Ƿ�����
					continue;
				if(in_range(my_hero.pos,info.my_units[m].pos,6.0)==1)
				{
					js++;		//��λ����
					if(info.my_units[m].hp<=info.my_units[m].Hp-150)
						js++;		//��λ���ˣ������ټ�һ
				}
			}
			if ((js>=2 /*&& my_hero.hp<=my_hero.Hp-40*/) || (my_hero.hp<=my_hero.Hp-40 && js>=1))		//�ж��Ƿ�����ʩ������(��������2����Ӣ�������ˡ���������1)
			{
				cmd.action=Action::MAGIC;return;
			}
		}
		//��λ����������******************************************************
		//���ез���λ�����ǣ����������£���󳷣�1,1�����������������������
		//������Χ�����޵з���λ���ǣ�������������
		//�жϵз�Ӣ���Ƿ��ڹ�����Χ�ڣ��ǣ��ж�ǿ����ǿ�����ˡ���վ�е�
		//if(dying_unit(info.enemy_units,my_hero,info.num_enemy_units)==1)  //���ܲ���ֱ�Ӳ�
		//{
		//	cmd.action=Action::ATTACK;
		//	cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);return;
		//}
		if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,3.0))		//�з���λ����
		{
			if(in_range(my_hero.pos,my_hero.id==0?point(45,13):point(63,31),3.0))	//������
			{
				cmd.action=Action::ATTACK;
				cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
				return;
			}
			else	//��������
			{
				cmd.action=Action::MOVE;
				cmd.pos=my_hero.id==0?point(45,13):point(63,31);
				return;
			}
		}		//�з���λδ����
		if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//������Χ���ез���λ
		{
			cmd.action=Action::ATTACK;
			cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
			return;
		}
		if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0)==0)	//������Χ��û�ез���λ
		{
			if(info.enemy_hero_vsb==true && in_range(my_hero.pos,info.enemy_hero.pos,6.0)==1)		//�з�Ӣ������Ұ��
			{
				if (hero_cmp()==1)		//�ҷ��޴�����
					{
						if (in_range(my_hero.pos,info.enemy_hero.pos,5.0)==1)
						{
						cmd.action=Action::ATTACK;
						cmd.pos=info.enemy_hero.pos;return;		//�����Է�Ӣ��
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
					return;		//�з�ǿ���˻ؼ���������
				}
			}
			else		//������Χ��ɶҲû��
			{
				cmd.action=Action::MOVE;
				cmd.pos=my_hero.id==0?point (58,26):point(50,18);
				return;
			}
		}
	}
	//����
	if(my_hero.level<=14)
	{
		if(my_hero.qui.cd_next==0 && my_hero.qui.mag_cos<=my_hero.mag)	//���ܿ��ͷ�ʱ
		{
			if (my_hero.hp<my_hero.Hp-100)
			{
				cmd.action=Action::MAGIC;return;//Ӣ��Ѫ����������-120ֱ�ӻ�Ѫ
			}
			for(m=0;m<info.num_my_units;m++)		//����ʩ����Χ�����˵�λ��
			{
				if(info.my_units[m].hp>info.my_units[m].Hp-180)		//�ж��ѷ���λ�Ƿ�����
					continue;
				if(in_range(my_hero.pos,info.my_units[m].pos,6.0)==1)
				{
					js++;		//��λ����
					if(info.my_units[m].hp<=info.my_units[m].Hp-250)
						js++;		//��λ���ˣ������ټ�һ
				}
			}
			if ((js>=2 /*&& my_hero.hp<=my_hero.Hp-80*/) || (my_hero.hp<=my_hero.Hp-70 && js>=1))		//�ж��Ƿ�����ʩ������(��������2����Ӣ�������ˡ���������1)
			{
				cmd.action=Action::MAGIC;return;
			}
		}
		//��������
		if (in_range(my_hero.pos,point(73,3),8.0)==0)
		{
			if (/*my_hero.hp/(60-my_hero.def)>yeguai_hp/(my_hero.atk-20) &&*/ yeguai_alive==1  && \
				in_range(nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,my_hero.pos,5.0)==0 && my_hero.hp>=my_hero.Hp*0.8)
				dayeguai=1;
		}
		if (dayeguai==0)
		{
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,3.0))		//�з���λ����
			{
				if(in_range(my_hero.pos,my_hero.id==0?point(45,13):point(63,31),3.0))	//������
				{
					cmd.action=Action::ATTACK;
					cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
					return;
				}
				else	//��������
				{
					cmd.action=Action::MOVE;
					cmd.pos=my_hero.id==0?point(45,13):point(63,31);
					return;
				}
			}		//�з���λδ����
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//������Χ���ез���λ
			{
				cmd.action=Action::ATTACK;
				cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
				return;
			}
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0)==0)	//������Χ��û�ез���λ
			{
				if(info.enemy_hero_vsb==true && in_range(my_hero.pos,info.enemy_hero.pos,6.0)==1)		//�з�Ӣ������Ұ��
				{
					if (hero_cmp()==1)		//�ҷ��޴�����
					{
						if (in_range(my_hero.pos,info.enemy_hero.pos,5.0)==1)
						{
						cmd.action=Action::ATTACK;
						cmd.pos=info.enemy_hero.pos;return;		//�����Է�Ӣ��
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
						return;		//�з�ǿ���˻ؼ���������
					}
				}
				else		//������Χ��ɶҲû��
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
			if ((my_hero.hp<=200 && info.enemy_hero_vsb==1 && in_range(my_hero.pos,info.enemy_hero.pos,5)) || yeguai_alive==0)			//����������ˣ���Ѫ��
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
				if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//������Χ���ез���λ
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
		if(my_hero.qui.cd_next==0 && my_hero.qui.mag_cos<=my_hero.mag)	//���ܿ��ͷ�ʱ
		{
			if (my_hero.hp<my_hero.Hp-100)
			{
				cmd.action=Action::MAGIC;return;//Ӣ��Ѫ����������-120ֱ�ӻ�Ѫ
			}
			for(m=0;m<info.num_my_units;m++)		//����ʩ����Χ�����˵�λ��
			{
				if(info.my_units[m].hp>info.my_units[m].Hp-180)		//�ж��ѷ���λ�Ƿ�����
					continue;
				if(in_range(my_hero.pos,info.my_units[m].pos,6.0)==1)
				{
					js++;		//��λ����
					if(info.my_units[m].hp<=info.my_units[m].Hp-250)
						js++;		//��λ���ˣ������ټ�һ
				}
			}
			if ((js>=2 /*&& my_hero.hp<=my_hero.Hp-80*/) || (my_hero.hp<=my_hero.Hp-70 && js>=1))		//�ж��Ƿ�����ʩ������(��������2����Ӣ�������ˡ���������1)
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
			if(in_range(my_hero.pos,nst_enemy_unit(info.enemy_units,my_hero,info.num_enemy_units).pos,5.0))		//������Χ���ез���λ
			{
				cmd.action=Action::ATTACK;
				cmd.pos=att_unit(info.enemy_units,my_hero,info.num_enemy_units);
				return;
			}
			if(in_range(nst_tower(info.enemy_towers,my_hero,info.num_enemy_towers).pos,my_hero.pos,5.0)==1)		//�������Թ���
			{
				cmd.action=Action::ATTACK;
				cmd.pos=nst_tower(info.enemy_towers,my_hero,info.num_enemy_towers).pos;
				return;
			}
			if(info.enemy_base_vsb==true && in_range(my_hero.pos,info.enemy_base.pos,my_hero.rng))		//�л��أ������
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

//�Զ��庯��������
Unit nst_enemy_unit(Unit enemy[],Hero hero,int num)		//����з���λ
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

point att_unit(Unit enemy[],Hero my_hero,int e_num)		//Ԥ��������λ���ݶ������λ��
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
		return 1;		//�ϻغ�δ���Է�Ӣ�ۣ����ϻغ�Ӣ��û�е�Ѫ��������û�е�Ѫ���ȴ�һ��
	else
	{
		if(temp_info.my_hero.hp/(history_info.my_hero.hp-temp_info.my_hero.hp)>=
			temp_info.enemy_hero.hp/((history_info.enemy_hero.hp-temp_info.enemy_hero.hp)<=0?\
			(temp_info.my_hero.atk - temp_info.enemy_hero.def):(history_info.enemy_hero.hp-temp_info.enemy_hero.hp)))
			return 1;		//�ҷ�Ӣ��ռ��
		else
			return 0;		//�Է�Ӣ��ռ��
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