/*
 * Copyright (C) 2010 Philippe Gerum <rpm@xenomai.org>.
 *
 * Xenomai is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Xenomai is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xenomai; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <linux/types.h>
#include <linux/ipipe.h>
#include <cobalt/kernel/thread.h>
#include <cobalt/uapi/syscall.h>
#include <asm/ptrace.h>

#ifdef CONFIG_X86_32

void xnarch_setup_mayday_page(void *page)
{
	/*
	 * We want this code to appear at the top of the MAYDAY page:
	 *
	 *	b8 2b 02 00 0c		mov    $<mux_code>,%eax
	 *      cd 80			int    $0x80
	 *	0f 0b			ud2a
	 *
	 * We intentionally don't mess with EFLAGS here, so that we
	 * don't have to save/restore it in handle/fixup code.
	 */
	static const struct __attribute__ ((__packed__)) {
		struct __attribute__ ((__packed__)) {
			u8 op;
			u32 imm;
		} mov_eax;
		u16 syscall;
		u16 bug;
	} code = {
		.mov_eax = {
			.op = 0xb8,
			.imm = __xn_syscode(sc_cobalt_mayday)
		},
		.syscall = 0x80cd,
		.bug = 0x0b0f,
	};

	memcpy(page, &code, sizeof(code));

	/* no cache flush required. */
}

#else /* CONFIG_X86_64 */

void xnarch_setup_mayday_page(void *page)
{
	/*
	 * We want this code to appear at the top of the MAYDAY page:
	 *
	 *	b8 2b 02 00 0c		mov    $<mux_code>,%eax
	 *	0f 05			syscall
	 *	0f 0b			ud2a
	 *
	 * We intentionally don't mess with EFLAGS here, so that we
	 * don't have to save/restore it in handle/fixup code.
	 */
	static const struct __attribute__ ((__packed__)) {
		struct __attribute__ ((__packed__)) {
			u8 op;
			u32 imm;
		} mov_eax;
		u16 syscall;
		u16 bug;
	} code = {
		.mov_eax = {
			.op = 0xb8,
			.imm = __xn_syscode(sc_cobalt_mayday)
		},
		.syscall = 0x050f,
		.bug = 0x0b0f,
	};

	memcpy(page, &code, sizeof(code));

	/* no cache flush required. */
}

#endif /* CONFIG_X86_64 */

void xnarch_handle_mayday(struct xnarchtcb *tcb, struct pt_regs *regs,
			  unsigned long tramp)
{
	tcb->mayday.sp = regs->sp;
	tcb->mayday.ip = regs->ip;
	tcb->mayday.ax = regs->ax;
	regs->ip = tramp;
}

void xnarch_fixup_mayday(struct xnarchtcb *tcb, struct pt_regs *regs)
{
	regs->ip = tcb->mayday.ip;
	regs->ax = tcb->mayday.ax;
	regs->sp = tcb->mayday.sp;
}

void xnarch_call_mayday(struct task_struct *p)
{
	ipipe_raise_mayday(p);
}
