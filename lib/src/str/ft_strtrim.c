/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcario <jcario@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/08 22:30:04 by jcario            #+#    #+#             */
/*   Updated: 2024/01/22 13:25:37 by jcario           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *str, char const *set)
{
	int	start;
	int	end;

	start = 0;
	end = ft_strlen(str) - 1;
	while (str[start] && ft_strchr((char *)set, str[start]))
		start++;
	while (end >= 0 && ft_strchr((char *)set, str[end]))
		end--;
	return (ft_substr(str, start, (end - start) + 1));
}
