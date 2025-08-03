/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcario <jcario@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 14:58:27 by jcario            #+#    #+#             */
/*   Updated: 2024/01/26 14:58:27 by jcario           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

long	ft_atoi_base(char *str)
{
	long	result = 0;
	int		base = 10;
	int		i = 0;
	int		sign = 1;

	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}

	if (str[i] == '0')
	{
		if (str[i + 1] == 'x' || str[i + 1] == 'X')
		{
			base = 16;
			i += 2;
		}
		else if (str[i + 1] == 'b' || str[i + 1] == 'B')
		{
			base = 2;
			i += 2;
		}
		else
			base = 8;
	}

	while (str[i])
	{
		char c = str[i];
		int value = -1;
		if (c >= '0' && c <= '9')
			value = c - '0';
		else if (c >= 'A' && c <= 'F')
			value = c - 'A' + 10;
		else if (c >= 'a' && c <= 'f')
			value = c - 'a' + 10;
		if (value == -1 || value >= base)
			break;
		result = result * base + value;
		i++;
	}
	return result * sign;
}
