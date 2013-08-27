from msp430.models import *
from django.contrib import admin
from django.forms import ModelForm, ChoiceField
from django.forms.models import BaseInlineFormSet
import json

class DisplayForm(ModelForm):
    # Overridden to dynamically filter list, otherwise its a integerfield
    channel_port = ChoiceField(label='channel/port')


class DisplayFormSet(BaseInlineFormSet):
    # different for input/output
    # filter based on IO type

    def add_fields(self, form, index):
        super(DisplayFormSet, self).add_fields(form, index)
        # read only interface
        if issubclass(self.model, ReadDisplay):
            if hasattr(form.instance, 'interface') and form.instance.interface is not None:
                interfaces = [form.instance.interface]
            else:
                interfaces = MSP430ReadInterface.objects.filter(msp430=self.instance)
        elif issubclass(self.model, WriteDisplay):
            if hasattr(form.instance, 'interface') and form.instance.interface is not None:
                interfaces = [form.instance.interface]
            else:
                interfaces = MSP430WriteInterface.objects.filter(msp430=self.instance)
        choices = []
        for interface in interfaces:
            if interface.io_type not in self.model.io_type:
                continue
            try:
                _choices = json.loads(interface.possible_choices)
            except:
                continue
            for item in _choices:
                choices.append((item['s'], interface.__unicode__() + ' ' + item['d']))
        form.fields['channel_port'].choices = choices

        # filter queryset
        form.fields['interface'].queryset = form.fields['interface'].queryset.filter(io_type__in=self.model.io_type)
        form.fields['interface'].queryset = form.fields['interface'].queryset.filter(msp430=self.instance)

class DisplayInline(admin.TabularInline):
    extra = 0
    formset = DisplayFormSet
    form = DisplayForm

class MyAdminForm(ModelForm):
    def __init__(self, *args, **kwargs):
        super(MyAdminForm, self).__init__(*args, **kwargs)

    class Meta:
        model = MSP430


"""Add new displays here
Input/output and return/input type are determined by the model definition
"""

class NumericDisplayInLine(DisplayInline):
    model = NumericDisplay

class ProgressBarDisplayInLine(DisplayInline):
    model = ProgressBarDisplay

class GraphDisplayInLine(DisplayInline):
    model = GraphDisplay

class ButtonDisplayInLine(DisplayInline):
    model = ButtonDisplay

class MSP430Admin(admin.ModelAdmin):
    readonly_fields = ('mac_address', 'current_ip', 'online')
    fields = ('name', 'mac_address', 'current_ip', 'online')
    form = MyAdminForm
    inlines = [NumericDisplayInLine, ProgressBarDisplayInLine, GraphDisplayInLine, ButtonDisplayInLine]

admin.site.register(MSP430, MSP430Admin)
