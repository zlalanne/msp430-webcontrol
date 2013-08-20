from django.db import models

# Create your models here.
class msp430(models.Model):
    name = models.CharField(max_length=200)
    mac_address = models.CharField(max_length=17, primary_key=True)
    current_ip = models.IPAddressField()
    online = models.BooleanField(default=False)

    def __str__(self):
        return self.name


