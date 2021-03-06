// Copyright (C) 2010 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <stdio.h>
#include "jellyfish.h"
#include <stdlib.h>
#include "core/msg.h"

jellyfish::jellyfish(vec3 *heap_ptr, u32 heap_size) :
    m_heap(heap_ptr),
    m_heap_size(heap_size)
{
    m_instruction = new bool[m_heap_size];

	for (u32 n=0; n<m_heap_size; n++)
	{
		m_heap[n]=vec3(0,0,0);
	}

	for (u32 n=0; n<m_heap_size; n++)
	{
		m_instruction[n]=false;
	}
}

jellyfish::~jellyfish()
{
}

vec3 jellyfish::peek(s32 addr) const
{
	return m_heap[addr%m_heap_size];
}

s32 jellyfish::peekix(s32 addr) const
{
    return (int)peek(addr).x;
}

void jellyfish::poke(s32 addr, const vec3 &data)
{	
	m_heap[addr%m_heap_size]=data;
}

bool jellyfish::is_instr(s32 addr) const
{
	return m_instruction[addr%m_heap_size];
}

void jellyfish::set_instr(s32 addr, bool s)
{
	m_instruction[addr%m_heap_size]=s;
}

void jellyfish::push(const vec3 &data)
{
//    printf("push %f %f %f\n",(float)(data.x),
//           (float)(data.y),
//           (float)(data.z));
    poke(peekix(REG_STP)+REG_STK,data);
    poke(REG_STP,peek(REG_STP)+vec3(1,0,0));
}

vec3 jellyfish::pop()
{
    poke(REG_STP,peek(REG_STP)-vec3(1,0,0));
    vec3 data=peek(peekix(REG_STP)+REG_STK);

///    printf("pop %f %f %f\n",(float)data.x,
//           (float)data.y,
//           (float)data.z);

    if (peekix(REG_STP)<0) poke(REG_STP,vec3(0,0,0));
    return data;
}

vec3 jellyfish::top()
{
    return peek(peekix(REG_STP)+REG_STK-1);
}

void jellyfish::run()
{
    // get pc
    s32 pc=peekix(REG_PCO)%m_heap_size;
    
    // fetch instruction
    vec3 c=peek(pc);
    s32 i=(int)c.x;
    s32 argiy=(int)c.y;
    s32 argiz=(int)c.z;

    set_instr(pc,true);

    //printf("%d",i);
    
	switch(i)
	{
    case NOP: break;
    case JMP: poke(REG_PCO,vec3(argiy-1,0,0)); break;
    case JMZ: if ((float)pop().x==0) poke(REG_PCO,vec3(pc+argiy,0,0)); break;
	case JLT: if (pop().x<pop().x) poke(REG_PCO,vec3(pc+argiy,0,0)); break;
    case JGT: if (pop().x>pop().x) poke(REG_PCO,vec3(pc+argiy,0,0)); break;
	case LDL: push(vec3(c.y,0,0)); break;
	case LDA: push(peek(argiy)+argiz); break;
	case LDI: push(peek(peekix(argiy)+argiz)); break;
	case STA: poke(argiy+argiz,pop()); break;
	case STI: poke(peekix(argiy)+argiz,pop()); break;
	case ADD: push(pop()+pop()); break;
	case SUB: push(pop()-pop()); break;
	case MUL: 
    {
        vec3 m=pop();
        vec3 v=pop();
        push(v*(float)m.x);
    } break;
	case DIV: push(pop()/(float)pop().x); break;
	case ABS: { vec3 t=pop(); push(vec3(fabs((float)t.x),
                                        fabs((float)t.y),
                                        fabs((float)t.z))); } break;
	case SIN: { flx_real a=pop().x; push(vec3(sin(((float)a)*0.0174532925),
                                              cos(((float)a)*0.0174532925),0));
    } break;
	case ATN: ; break;
	case DOT: push(vec3(pop().dot(pop()),0,0)); break;
	case CRS: push(pop().cross(pop())); break;
	case SQR: push(vec3(sqrt((float)pop().x),0,0)); break;
	case LEN: push(vec3(pop().mag(),0,0)); break;
	case DUP: push(top()); break;
	case CMP: push(vec3(pop().x,pop().x,pop().x)); break;
	case SHF: 
    { 
        vec3 shf=pop(); 
        vec3 src=pop();
        vec3 dst;        
        dst.x=(&src.x)[((int)fabs(shf.x))%3];
        if ((float)shf.x<0) dst.x=-dst.x;
        dst.y=(&src.x)[((int)fabs(shf.y))%3];
        if ((float)shf.y<0) dst.y=-dst.y;
        dst.z=(&src.x)[((int)fabs(shf.z))%3];
        if ((float)shf.z<0) dst.z=-dst.z;
        push(dst);
    } break;
	case BLD: 
    {
        vec3 ret;
        for (int i=0; i<3; i++)
        {
            vec3 t=peek(argiy);
            ret.x=(&t.x)[argiz];
        }
        push(ret);
    } break;
	case RET: poke(REG_PCO,vec3((int)pop().x,0,0)); break;
    default: set_instr(pc,false);
   	};



//    print_instr(pc);
//    printf("\n");
    
    // inc pc 
    // todo - speed?
    poke(REG_PCO,peek(REG_PCO)+vec3(1,0,0));
}

void jellyfish::simple_dump() const
{
    s32 pc=peekix(REG_PCO)%m_heap_size;
	for (u32 n=0; n<m_heap_size; n++)
	{
        if (n==pc)
        {
            printf("> ");
        }
        else
        {
            printf("  ");
        }

        print_instr(n);
        printf(" : %d\n",n);
	}
}

void jellyfish::pretty_dump() const
{
    // get pc
    s32 pc=peekix(REG_PCO)%m_heap_size;
    s32 stop=peekix(REG_STP);

    printf("-- prog -- pc:%d\n",pc);

	for (s32 n=pc-7; n<pc+7; n++)
	{
        if (n>=0 || n<m_heap_size)
        {
            if (n==pc)
            {
                printf("%03d > ",n);
            }
            else
            {
                printf("%03d   ",n);
            }
            
            print_instr(n);
            printf("\n");
        }
	}

    printf("-- stk -- stk:%d\n",stop);
	for (u32 n=REG_STK+stop-2; n<REG_STK+stop+2; n++)
	{
        char txt[2048];
        if (n==stop+REG_STK)
        {
            sprintf(txt,"%s","> %f %f %f : %d\n");
        }
        else
        {
            sprintf(txt,"%s","  %f %f %f : %d\n");
        }

		printf(txt,
               (float)m_heap[n].x,
               (float)m_heap[n].y,
               (float)m_heap[n].z,
               n);
	}

}

void jellyfish::print_instr(s32 addr) const
{
    vec3 i=peek(addr);

    if (m_instruction[addr])
    {
        
        switch((int)i.x)
        {
        case NOP: printf("nop"); break;
        case JMP: printf("jmp"); break;
        case JMZ: printf("jmz"); break;
        case JLT: printf("jlt"); break;
        case JGT: printf("jgt"); break;
        case LDL: printf("ldl"); break;
        case LDA: printf("lda"); break;
        case LDI: printf("ldi"); break;
        case STA: printf("sta"); break;
        case STI: printf("sti"); break;
        case ADD: printf("add"); break;
        case SUB: printf("sub"); break;
        case MUL: printf("mul"); break;
        case DIV: printf("div"); break;
        case ABS: printf("abs"); break;
        case SIN: printf("sin"); break;
        case ATN: printf("atn"); break;
        case DOT: printf("dot"); break;
        case CRS: printf("crs"); break;
        case SQR: printf("sqr"); break;
        case LEN: printf("len"); break;
        case DUP: printf("dup"); break;
        case CMP: printf("cmp"); break;
        case SHF: printf("shf"); break;
        case BLD: printf("bld"); break;
        case RET: printf("ret"); break;
        default: printf("%f", (float)i.x); break;
        };

        printf(" %f %f",(float)i.y,(float)i.z);
    }
    else printf("%f %f %f",
                (float)i.x,
                (float)i.y,
                (float)i.z);
}

void jellyfish::trash()
{
    for (int i=0; i<m_heap_size; i++)
    {
        poke(i,vec3((rand()%1000/1000.0)*25,
                    (rand()%1000/1000.0)*25,
                    (rand()%1000/1000.0)*25));
    }
}
