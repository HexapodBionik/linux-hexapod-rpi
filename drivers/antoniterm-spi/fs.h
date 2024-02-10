#ifndef FS_H_
#define FS_H_

int fs_class_setup(void);
void fs_class_destroy(void);
struct device *fs_create_device_and_config(int8_t antoniterm_num, struct antoniterm_device *antoniterm_dev);

#endif /* FS_H_ */
