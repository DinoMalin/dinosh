/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthexa_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcario <jcario@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 19:01:05 by jcario            #+#    #+#             */
/*   Updated: 2024/01/12 15:31:11 by jcario           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_puthexa_fd(unsigned long n, int fd, int up)
{
	static char	hexa[17] = "0123456789abcdef";

	if (n >= 16)
		return (ft_puthexa_fd(n / 16, fd, up) + ft_puthexa_fd(n % 16, fd, up));
	else if (n < 16)
	{
		if (up)
			return (ft_putchar_fd(ft_toupper(hexa[n]), fd));
		else
			return (ft_putchar_fd(hexa[n], fd));
	}
	return (0);
}
