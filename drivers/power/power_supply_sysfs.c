/*
 *  Sysfs interface for the universal power supply monitor class
 *
 *  Copyright © 2007  David Woodhouse <dwmw2@infradead.org>
 *  Copyright © 2007  Anton Vorontsov <cbou@mail.ru>
 *  Copyright © 2004  Szabolcs Gyurko
 *  Copyright © 2003  Ian Molton <spyro@f2s.com>
 *
 *  Modified: 2004, Oct     Szabolcs Gyurko
 *
 *  You may use this code as per GPL version 2
 */

#include <linux/ctype.h>
#include <linux/power_supply.h>

#include "power_supply.h"

/*
 * This is because the name "current" breaks the device attr macro.
 * The "current" word resolves to "(get_current())" so instead of
 * "current" "(get_current())" appears in the sysfs.
 *
 * The source of this definition is the device.h which calls __ATTR
 * macro in sysfs.h which calls the __stringify macro.
 *
 * Only modification that the name is not tried to be resolved
 * (as a macro let's say).
 */
#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */
/* S_S [woonghee] 2009-09-25, battery charging */
#include <mach/msm_battery.h>
#include <mach/msm_battery_thunderc.h>
#endif

/*
 * This is because the name "current" breaks the device attr macro.
 * The "current" word resolves to "(get_current())" so instead of
 * "current" "(get_current())" appears in the sysfs.
 *
 * The source of this definition is the device.h which calls __ATTR
 * macro in sysfs.h which calls the __stringify macro.
 *
 * Only modification that the name is not tried to be resolved
 * (as a macro let's say).
 */
#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */
/* S_S [woonghee] 2009-09-25, battery charging */
#define PSEUDO_BATT_ATTR(_name)					\
{									\
	.attr = { .name = #_name, .mode = 0666 },	\
	.show = pseudo_batt_show_property,				\
	.store = pseudo_batt_store_property,							\
}
/* 
 * Support block charging for Q-gate
 * 2010-07-18, taehung.kim
 */
#define BLOCK_CHARGING_ATTR(_name)					\
{									\
	.attr = { .name = #_name, .mode = 0666 },	\
	.show = block_charging_show_property,				\
	.store = block_charging_store_property,							\
}
#endif

#define POWER_SUPPLY_ATTR(_name)					\
{									\
	.attr = { .name = #_name, .mode = 0444 },	\
	.show = power_supply_show_property,				\
	.store = NULL,							\
}

static struct device_attribute power_supply_attrs[];

static ssize_t power_supply_show_property(struct device *dev,
					  struct device_attribute *attr,
					  char *buf) {
	static char *status_text[] = {
		"Unknown", "Charging", "Discharging", "Not charging", "Full"
	};
	static char *charge_type[] = {
		"Unknown", "N/A", "Trickle", "Fast"
	};
	static char *health_text[] = {
		"Unknown", "Good", "Overheat", "Dead", "Over voltage",
		"Unspecified failure", "Cold",
	};
	static char *technology_text[] = {
		"Unknown", "NiMH", "Li-ion", "Li-poly", "LiFe", "NiCd",
		"LiMn"
	};
	static char *capacity_level_text[] = {
		"Unknown", "Critical", "Low", "Normal", "High", "Full"
	};
	ssize_t ret;
	struct power_supply *psy = dev_get_drvdata(dev);
	const ptrdiff_t off = attr - power_supply_attrs;
	union power_supply_propval value;

	ret = psy->get_property(psy, off, &value);

	if (ret < 0) {
		if (ret != -ENODEV)
			dev_err(dev, "driver failed to report `%s' property\n",
				attr->attr.name);
		return ret;
	}

	if (off == POWER_SUPPLY_PROP_STATUS)
		return sprintf(buf, "%s\n", status_text[value.intval]);
	else if (off == POWER_SUPPLY_PROP_CHARGE_TYPE)
		return sprintf(buf, "%s\n", charge_type[value.intval]);
	else if (off == POWER_SUPPLY_PROP_HEALTH)
		return sprintf(buf, "%s\n", health_text[value.intval]);
	else if (off == POWER_SUPPLY_PROP_TECHNOLOGY)
		return sprintf(buf, "%s\n", technology_text[value.intval]);
	else if (off == POWER_SUPPLY_PROP_CAPACITY_LEVEL)
		return sprintf(buf, "%s\n", capacity_level_text[value.intval]);
#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */
	/* S_S [woonghee.park] 2010-02-09, [VS740] */
	else if (off >= POWER_SUPPLY_PROP_MODEL_NAME && off <= POWER_SUPPLY_PROP_SERIAL_NUMBER)
#else
	else if (off >= POWER_SUPPLY_PROP_MODEL_NAME)
#endif
		return sprintf(buf, "%s\n", value.strval);

	return sprintf(buf, "%d\n", value.intval);
}

#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */

/* S_S [woonghee] 2009-09-25, battery charging */
static ssize_t pseudo_batt_show_property(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t ret;
	struct power_supply *psy = dev_get_drvdata(dev);
	const ptrdiff_t off = attr - power_supply_attrs;
	union power_supply_propval value;

	static char *pseudo_batt[] = {
		"NORMAL", "PSEUDO",
	};

	ret = psy->get_property(psy, off, &value);

	if (ret < 0) {
		if (ret != -ENODEV)
			dev_err(dev, "driver failed to report `%s' property\n",
					attr->attr.name);
		return ret;
	}
	if (off == POWER_SUPPLY_PROP_PSEUDO_BATT)
		return sprintf(buf, "[%s] \nusage: echo [mode] [ID] [therm] [temp] [volt] [cap] [charging] > pseudo_batt\n", pseudo_batt[value.intval]);

	return 0;
}

extern int pseudo_batt_set(struct pseudo_batt_info_type*);

static ssize_t pseudo_batt_store_property(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int ret = -EINVAL;
	struct pseudo_batt_info_type info;

	if (sscanf(buf, "%d %d %d %d %d %d %d", &info.mode, &info.id, &info.therm,
				&info.temp, &info.volt, &info.capacity, &info.charging) != 7)
	{
		if(info.mode == 1) //pseudo mode
		{
			printk(KERN_ERR "usage : echo [mode] [ID] [therm] [temp] [volt] [cap] [charging] > pseudo_batt");
			goto out;
		}
	}
	pseudo_batt_set(&info);
	ret = count;
out:
	return ret;
}

/* LGE_CHNAGE
 * Support block charging for Q-gate
 * 2010-07-18, taehung.kim
 */
extern void batt_block_charging_set(int);
static ssize_t block_charging_store_property(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	int ret = -EINVAL;
	int block;
	

	if(sscanf(buf, "%d",&block) != 1)
	{
		printk("%s:Too many argument\n",__func__);
		goto out;
	}
	printk("%s:block charging=%d\n",__func__,block);
	batt_block_charging_set(block);
	ret = count;
out:
	return ret;
}
static ssize_t block_charging_show_property(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t ret;
	struct power_supply *psy = dev_get_drvdata(dev);
	const ptrdiff_t off = attr - power_supply_attrs;
	union power_supply_propval value;

	static char *block_charging_mode[] = {
		"BLOCK CHARGING", "NORMAL",
	};

	ret = psy->get_property(psy, off, &value);

	if (ret < 0) {
		if (ret != -ENODEV)
			dev_err(dev, "driver failed to report `%s' property\n",
					attr->attr.name);
		return ret;
	}

	if (off == POWER_SUPPLY_PROP_BLOCK_CHARGING)
		return sprintf(buf, "[%s]", block_charging_mode[value.intval]);

	return 0;
}
#endif

/* Must be in the same order as POWER_SUPPLY_PROP_* */
static struct device_attribute power_supply_attrs[] = {
	/* Properties of type `int' */
	POWER_SUPPLY_ATTR(status),
	POWER_SUPPLY_ATTR(charge_type),
	POWER_SUPPLY_ATTR(health),
	POWER_SUPPLY_ATTR(present),
	POWER_SUPPLY_ATTR(online),
	POWER_SUPPLY_ATTR(technology),
	POWER_SUPPLY_ATTR(voltage_max),
	POWER_SUPPLY_ATTR(voltage_min),
	POWER_SUPPLY_ATTR(voltage_max_design),
	POWER_SUPPLY_ATTR(voltage_min_design),
#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || \
	defined (CONFIG_MACH_MSM7X27_GISELE) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */
	/* S_S [woonghee]	2009-09-25, battery charging */
	POWER_SUPPLY_ATTR(batt_vol),
#else	/* origin */
	POWER_SUPPLY_ATTR(voltage_now),
#endif	
	POWER_SUPPLY_ATTR(voltage_avg),
	POWER_SUPPLY_ATTR(current_now),
	POWER_SUPPLY_ATTR(current_avg),
	POWER_SUPPLY_ATTR(power_now),
	POWER_SUPPLY_ATTR(power_avg),
	POWER_SUPPLY_ATTR(charge_full_design),
	POWER_SUPPLY_ATTR(charge_empty_design),
	POWER_SUPPLY_ATTR(charge_full),
	POWER_SUPPLY_ATTR(charge_empty),
	POWER_SUPPLY_ATTR(charge_now),
	POWER_SUPPLY_ATTR(charge_avg),
	POWER_SUPPLY_ATTR(charge_counter),
	POWER_SUPPLY_ATTR(energy_full_design),
	POWER_SUPPLY_ATTR(energy_empty_design),
	POWER_SUPPLY_ATTR(energy_full),
	POWER_SUPPLY_ATTR(energy_empty),
	POWER_SUPPLY_ATTR(energy_now),
	POWER_SUPPLY_ATTR(energy_avg),
	POWER_SUPPLY_ATTR(capacity),
	POWER_SUPPLY_ATTR(capacity_level),
#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || \
	defined (CONFIG_MACH_MSM7X27_GISELE) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */
	/* S_S [woonghee]	2009-09-25, battery charging */
	/* need to match sysfs name, see BATTERY_TEMPERATURE_PATH */
	POWER_SUPPLY_ATTR(batt_temp),
#else
	POWER_SUPPLY_ATTR(temp),
#endif
	POWER_SUPPLY_ATTR(temp_ambient),
	POWER_SUPPLY_ATTR(time_to_empty_now),
	POWER_SUPPLY_ATTR(time_to_empty_avg),
	POWER_SUPPLY_ATTR(time_to_full_now),
	POWER_SUPPLY_ATTR(time_to_full_avg),
	/* Properties of type `const char *' */
	POWER_SUPPLY_ATTR(model_name),
	POWER_SUPPLY_ATTR(manufacturer),
	POWER_SUPPLY_ATTR(serial_number),
#if defined (CONFIG_MACH_MSM7X27_ALOHAV) || defined (CONFIG_MACH_MSM7X27_THUNDERC)
/* LGE_CHNAGE
 * ADD THUNDERC feature to use VS740 BATT DRIVER IN THUNDERC
 * 2010-05-13, taehung.kim
 */
	/* S_S [woonghee.park] 2010-02-09, [VS740] */
	POWER_SUPPLY_ATTR(valid_batt_id),
	POWER_SUPPLY_ATTR(batt_therm),
	PSEUDO_BATT_ATTR(pseudo_batt),
/* LGE_CHNAGE
 * Support block charging for Q-gate
 * 2010-07-18, taehung.kim
 */
	BLOCK_CHARGING_ATTR(block_charging),//43
#endif
};

static ssize_t power_supply_show_static_attrs(struct device *dev,
					      struct device_attribute *attr,
					      char *buf) {
	static char *type_text[] = { "Battery", "UPS", "Mains", "USB" };
	struct power_supply *psy = dev_get_drvdata(dev);

	return sprintf(buf, "%s\n", type_text[psy->type]);
}

static struct device_attribute power_supply_static_attrs[] = {
	__ATTR(type, 0444, power_supply_show_static_attrs, NULL),
};

int power_supply_create_attrs(struct power_supply *psy)
{
	int rc = 0;
	int i, j;

	for (i = 0; i < ARRAY_SIZE(power_supply_static_attrs); i++) {
		rc = device_create_file(psy->dev,
			    &power_supply_static_attrs[i]);
		if (rc)
			goto statics_failed;
	}

	for (j = 0; j < psy->num_properties; j++) {
		rc = device_create_file(psy->dev,
			    &power_supply_attrs[psy->properties[j]]);
		if (rc)
			goto dynamics_failed;
	}

	goto succeed;

dynamics_failed:
	while (j--)
		device_remove_file(psy->dev,
			   &power_supply_attrs[psy->properties[j]]);
statics_failed:
	while (i--)
		device_remove_file(psy->dev, &power_supply_static_attrs[i]);
succeed:
	return rc;
}

void power_supply_remove_attrs(struct power_supply *psy)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(power_supply_static_attrs); i++)
		device_remove_file(psy->dev, &power_supply_static_attrs[i]);

	for (i = 0; i < psy->num_properties; i++)
		device_remove_file(psy->dev,
			    &power_supply_attrs[psy->properties[i]]);
}

static char *kstruprdup(const char *str, gfp_t gfp)
{
	char *ret, *ustr;

	ustr = ret = kmalloc(strlen(str) + 1, gfp);

	if (!ret)
		return NULL;

	while (*str)
		*ustr++ = toupper(*str++);

	*ustr = 0;

	return ret;
}

int power_supply_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	int ret = 0, j;
	char *prop_buf;
	char *attrname;

	dev_dbg(dev, "uevent\n");

	if (!psy || !psy->dev) {
		dev_dbg(dev, "No power supply yet\n");
		return ret;
	}

	dev_dbg(dev, "POWER_SUPPLY_NAME=%s\n", psy->name);

	ret = add_uevent_var(env, "POWER_SUPPLY_NAME=%s", psy->name);
	if (ret)
		return ret;

	prop_buf = (char *)get_zeroed_page(GFP_KERNEL);
	if (!prop_buf)
		return -ENOMEM;

	for (j = 0; j < ARRAY_SIZE(power_supply_static_attrs); j++) {
		struct device_attribute *attr;
		char *line;

		attr = &power_supply_static_attrs[j];

		ret = power_supply_show_static_attrs(dev, attr, prop_buf);
		if (ret < 0)
			goto out;

		line = strchr(prop_buf, '\n');
		if (line)
			*line = 0;

		attrname = kstruprdup(attr->attr.name, GFP_KERNEL);
		if (!attrname) {
			ret = -ENOMEM;
			goto out;
		}

		dev_dbg(dev, "Static prop %s=%s\n", attrname, prop_buf);

		ret = add_uevent_var(env, "POWER_SUPPLY_%s=%s", attrname, prop_buf);
		kfree(attrname);
		if (ret)
			goto out;
	}

	dev_dbg(dev, "%zd dynamic props\n", psy->num_properties);

	for (j = 0; j < psy->num_properties; j++) {
		struct device_attribute *attr;
		char *line;

		attr = &power_supply_attrs[psy->properties[j]];

		ret = power_supply_show_property(dev, attr, prop_buf);
		if (ret == -ENODEV) {
			/* When a battery is absent, we expect -ENODEV. Don't abort;
			   send the uevent with at least the the PRESENT=0 property */
			ret = 0;
			continue;
		}

		if (ret < 0)
			goto out;

		line = strchr(prop_buf, '\n');
		if (line)
			*line = 0;

		attrname = kstruprdup(attr->attr.name, GFP_KERNEL);
		if (!attrname) {
			ret = -ENOMEM;
			goto out;
		}

		dev_dbg(dev, "prop %s=%s\n", attrname, prop_buf);

		ret = add_uevent_var(env, "POWER_SUPPLY_%s=%s", attrname, prop_buf);
		kfree(attrname);
		if (ret)
			goto out;
	}

out:
	free_page((unsigned long)prop_buf);

	return ret;
}
