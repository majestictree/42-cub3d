/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ncolomer <ncolomer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/31 12:53:02 by ncolomer          #+#    #+#             */
/*   Updated: 2019/11/01 19:11:14 by ncolomer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "engine.h"

t_window
	*new_window(t_config *config)
{
	t_window	*window;

	if (!(window = (t_window*)malloc(sizeof(*window))))
		return (NULL);
	window->width = config->requested_width;
	window->height = config->requested_height;
	if (!(window->ptr = mlx_init())
		|| !(window->win =	mlx_new_window(
			window->ptr,
			window->width,
			window->height,
			"cub3d"))
		)
	{
		free(window);
		return (NULL);
	}
	window->origin.x = 0;
	window->origin.y = 0;
	window->size.x = window->width;
	window->size.y = window->height;
	return (window);
}

int
	clear_window(t_window *window)
{
	return (mlx_clear_window(window->ptr, window->win));
}

int
	shade_color(int color, double divide)
{
	int	new;

	if (divide <= 1.)
		return (color);
	new = ((int)(((0xFF0000 & color) >> 16) / divide) << 16)
		+ ((int)(((0x00FF00 & color) >> 8) / divide) << 8)
		+ ((int)((0x0000FF & color) / divide));
	return (new);
}

static void
	draw_sky_floor(t_game *game)
{
	(void)game;
	/*int		i;
	t_pos	p;
	t_pos	wh;
	int		shade_height;

	shade_height = game->window->height / 20.;
	set_pos(&wh, game->window->width, shade_height);
	p.x = 0;
	i = 0;
	while (i < 10)
	{
		p.y = i * shade_height;
		draw_rectangle(game->window, &p, &wh,
			shade_color(game->config->sky_color, 1. + (.5 * i)));
		p.y = game->window->height - ((i + 1) * shade_height);
		draw_rectangle(game->window, &p, &wh,
			shade_color(game->config->floor_color, 1. + (.5 * i)));
		i++;
	}*/
}

/*static int
	dda_hori(t_pos *pos)
{
	int	length;

	//if ()
}*/

static void
	ray_cast(t_game *game, t_raysult *ray, t_pos *pos, double angle)
{
	int		hit;
	t_pos	l_pos;
	t_pos	sin_cos;
	t_pos	map_pos;
	t_pos	difference;
	t_pos	direction;

	(void)ray;
	copy_pos(&l_pos, pos);
	set_pos(&map_pos, (int)pos->x, (int)pos->y);
	set_pos(&sin_cos, sin(angle), cos(angle));
	printf("{difference %lfx%lfy}\n", difference.x, difference.y);
	printf("{direction %lfx%lfy}\n", direction.x, direction.y);
	hit = 0;
	while (!hit)
	{
		set_pos(&difference, l_pos.x - map_pos.x + cos(angle),
							l_pos.y - map_pos.y + sin(angle));
		if (difference.x > 0)
			l_pos.x += 1;
		else
			l_pos.x += -1;
		if (difference.y > 0)
			l_pos.y += 1;
		else
			l_pos.y += -1;
		set_pos(&map_pos, (int)l_pos.x, (int)l_pos.y);
		if (MAP(map_pos, game->config) == '1')
			hit = 1;
	}
	printf("{wall hit %lfx%lfy}\n", map_pos.x, map_pos.y);
	if (map_pos.x - pos->x >= map_pos.y - pos->y)
		ray->distance = map_pos.x - pos->x;
	else
		ray->distance = map_pos.y - pos->y;
	ray->distance *= cos(angle);
}

void
	update_window(t_game *game)
{
	int			i;
	t_raysult	ray;
	double		angle_step;
	t_pos		start;
	t_pos		end;

	set_pos(&start, 0, 0);
	set_pos(&end, 0, 0);
	clear_window(game->window);
	draw_sky_floor(game);
	i = 0;
	while (i < game->window->width)
	{
		// focal_length = 0.8 ?
		angle_step = atan2(((double)i / game->window->width) - .5, 0.8);
		ray_cast(game, &ray, &game->camera->pos, game->camera->angle + angle_step);
		int height = game->window->height / ray.distance;
		set_pos(&start, i, game->window->height / 2 - height / 2);
		set_pos(&end, i, game->window->height / 2 + height / 2);
		draw_line(game->window, &start, &end, 0xFFFFFF);
		i++;
	}
}
