/*
 * IMAGE
 */

static PyObject *
pg_image_get_rect(pgImageObject *self) {
    return Py_BuildValue("O", self->srcrect);
}

static PyObject *
pg_image_draw(pgImageObject *self, PyObject *args, PyObject *kw) {
    PyObject* srcrect = NULL;
    PyObject* dstrect = NULL;

    SDL_Rect src;
    SDL_Rect dst;
    SDL_Rect *csrcrect = NULL;
    SDL_Rect *cdstrect = NULL;
    SDL_Point origin;
    SDL_Rect *rectptr = NULL;
    int px, py;

    static char *keywords[] = {
        "srcrect",
        "dstrect",
        NULL
    };

    if(!PyArg_ParseTupleAndKeywords(args, kw, "|OO", keywords, &srcrect, &dstrect)) {
        return NULL;
    }

    if (!srcrect) {
        csrcrect = &pgRect_AsRect(self->srcrect);
    }
    else {
        rectptr = pgRect_FromObject(srcrect, &src);
        if (!rectptr) {
            RAISE(PyExc_TypeError, "srcrect must be a rect or None.");
            return -1;
        }
        src.x = rectptr->x;
        src.y = rectptr->y;
        src.w = rectptr->w;
        src.h = rectptr->h;

        src.x += self->srcrect->r.x;
        src.y += self->srcrect->r.y;
        csrcrect = &src;      
    }

    //TODO: what if dstrect is None?
    if (dstrect) {
        cdstrect = pgRect_FromObject(dstrect, &dst);
        if (!cdstrect) {
            if (pg_TwoIntsFromObj(dstrect, &px, &py)) {
                dst.x = px;
                dst.y = py;
                dst.w = self->srcrect->r.w;
                dst.h = self->srcrect->r.h;
                cdstrect = &dst;
            }
            else {
                RAISE(PyExc_TypeError, "dstrect must be a position, rect, or None.");
                return -1;                
            }
        }
    }

    //TODO: bad to circumvent getters and setters? Especially when obj life cycles are being managed.
    self->texture->color = self->color;
    self->texture->alpha = self->alpha;

    origin.x = self->originx;
    origin.y = self->originy;

    texture_draw_internal(self->texture, csrcrect, cdstrect, self->angle, &origin, self->flipX, self->flipY);
    Py_RETURN_NONE;
}

static PyMethodDef pg_image_methods[] = {
    { "get_rect", (PyCFunction)pg_image_get_rect, METH_NOARGS, "TODO"},
    { "draw", (PyCFunction)pg_image_draw, METH_VARARGS | METH_KEYWORDS, "TODO"},
    { NULL }
};

static PyObject *
pg_image_get_angle(pgImageObject *self) 
{
    return Py_BuildValue("f", self->angle);
}

static PyObject *
pg_image_set_angle(pgImageObject *self, PyObject *val, void *closure) 
{
    float angle;
    if(!pg_FloatFromObj(val, &angle)) {
        RAISE(PyExc_TypeError, "angle should be a float.");
        return -1;         
    }
    self->angle = angle;
    return 0;
}

static PyObject *
pg_image_get_origin(pgImageObject *self) 
{
    return Py_BuildValue("(ff)", self->originx, self->originy);
}

static PyObject *
pg_image_set_origin(pgImageObject *self, PyObject *val, void *closure) 
{
    float x, y;
    if(!pg_TwoFloatsFromObj(val, &x, &y)) {
        RAISE(PyExc_TypeError, "Origin should be a sequence or two floats or ints.");
        return -1;         
    }
    self->originx = x;
    self->originy = y;
    return 0;
}

static PyObject *
pg_image_get_flipX(pgImageObject *self) 
{
    if (self->flipX)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject *
pg_image_set_flipX(pgImageObject *self, PyObject *val, void *closure) 
{
    int flipX = PyObject_IsTrue(val); // TODO: is this crazy? allows stuff like `Image.flipX = "wow"`
    self->flipX = flipX;
}

static PyObject *
pg_image_get_flipY(pgImageObject *self) 
{
    if (self->flipY)
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

static PyObject *
pg_image_set_flipY(pgImageObject *self, PyObject *val, void *closure) 
{
    int flipY = PyObject_IsTrue(val); // TODO: is this crazy? allows stuff like `Image.flipY = "wow"`
    self->flipY = flipY;
}

static PyObject *
pg_image_get_color(pgImageObject *self) 
{
    return Py_BuildValue("O", self->color);
}

static PyObject *
pg_image_set_color(pgImageObject *self, PyObject *val, void *closure) 
{
    //TODO: life cycle management and checking its a color object
    self->color = (pgColorObject*) val;
    Py_INCREF(self->color);
    return 0;
}

static PyObject *
pg_image_get_alpha(pgImageObject *self) 
{
    return Py_BuildValue("f", self->alpha);
}

static PyObject *
pg_image_set_alpha(pgImageObject *self, PyObject *val, void *closure) 
{
    float alpha;
    if(!pg_FloatFromObj(val, &alpha)) {
        RAISE(PyExc_TypeError, "brightness should be a float.");
        return -1;         
    }
    self->alpha = alpha;
    return 0;
}

static PyObject *
pg_image_get_texture(pgImageObject *self) 
{
    return Py_BuildValue("O", self->texture);
}

static PyObject *
pg_image_set_texture(pgImageObject *self, PyObject *val, void *closure) 
{
    //TODO: check if it is valid
    //TODO: manage life cycle
    self->texture = val;
    Py_INCREF(self->texture);
    return 0;
}

static PyObject *
pg_image_get_srcrect(pgImageObject *self) 
{
    return Py_BuildValue("O", self->srcrect);
}

static PyObject *
pg_image_set_srcrect(pgImageObject *self, PyObject *val, void *closure) 
{
    //TODO: check if it is valid
    self->srcrect = val;
    Py_INCREF(self->srcrect);
    return 0;
}

static PyGetSetDef pg_image_getset[] = {
    { "angle", (getter)pg_image_get_angle, (setter)pg_image_set_angle, NULL /*TODO*/, NULL },
    { "origin", (getter)pg_image_get_origin, (setter)pg_image_set_origin, NULL /*TODO*/, NULL },
    { "flipX", (getter)pg_image_get_flipX, (setter)pg_image_set_flipX, NULL /*TODO*/, NULL },
    { "flipY", (getter)pg_image_get_flipY, (setter)pg_image_set_flipY, NULL /*TODO*/, NULL },
    { "color", (getter)pg_image_get_color, (setter)pg_image_set_color, NULL /*TODO*/, NULL },
    { "alpha", (getter)pg_image_get_alpha, (setter)pg_image_set_alpha, NULL /*TODO*/, NULL },
    { "texture", (getter)pg_image_get_texture, (setter)pg_image_set_texture, NULL /*TODO*/, NULL },
    { "srcrect", (getter)pg_image_get_srcrect, (setter)pg_image_set_srcrect, NULL /*TODO*/, NULL },
    { NULL }
};

static int
pg_image_init(pgImageObject *self, PyObject *args, PyObject *kw) 
{

    /*
        cdef SDL_Rect temp
        cdef SDL_Rect *rectptr

        if isinstance(textureOrImage, Image):
            self.texture = textureOrImage.texture
            self.srcrect = pgRect_New(&(<Rect>textureOrImage.srcrect).r)
        else:
            self.texture = textureOrImage
            self.srcrect = textureOrImage.get_rect()

        if srcrect is not None:
            rectptr = pgRect_FromObject(srcrect, &temp)
            if rectptr == NULL:
                raise TypeError('srcrect must be None or a rectangle')
            temp.x = rectptr.x
            temp.y = rectptr.y
            temp.w = rectptr.w
            temp.h = rectptr.h

            if temp.x < 0 or temp.y < 0 or \
                temp.w < 0 or temp.h < 0 or \
                temp.x + temp.w > self.srcrect.w or \
                temp.y + temp.h > self.srcrect.h:
                raise ValueError('rect values are out of range')
            temp.x += self.srcrect.x
            temp.y += self.srcrect.y
            self.srcrect = pgRect_New(&temp)

        self.origin[0] = self.srcrect.w / 2
        self.origin[1] = self.srcrect.h / 2
    */

    SDL_Rect temp;
    SDL_Rect *rectptr;
    PyObject *srcrect = NULL;
    PyObject *textureOrImage;

    char* keywords[] = {
        "textureOrImage",
        "srcrect",
        NULL
    };

    if (!PyArg_ParseTupleAndKeywords(args, kw, "O|O", keywords,
                                     &textureOrImage, &srcrect)) {
        return NULL;
    }

    if (pgTexture_Check(textureOrImage)) {
        self->texture = textureOrImage;
        self->srcrect = pg_texture_get_rect(textureOrImage, NULL, NULL);
    }
    else if (pgImage_Check(textureOrImage)) {
        self->texture = ((pgImageObject*) textureOrImage)->texture;
        self->srcrect = ((pgImageObject*) textureOrImage)->srcrect;
    }
    else {
        return RAISE(PyExc_TypeError, "textureOrImage... must be a Texture or an Image.");
    }
    Py_INCREF(self->texture);

    if (srcrect) {
        rectptr = pgRect_FromObject(srcrect, &temp);
        if (!rectptr) {
            RAISE(PyExc_TypeError, "srcrect must a rectangle or None");
            return -1;
        }
        temp.x = rectptr->x;
        temp.y = rectptr->y;
        temp.w = rectptr->w;
        temp.h = rectptr->h;

        if (temp.x < 0 || temp.y < 0 || temp.w < 0 || temp.h < 0 ||
            temp.x + temp.w > rectptr->w || temp.y + temp.h > rectptr->h) {
            RAISE(PyExc_ValueError, "rect values are out of range");
            return -1;
        }

        temp.x += rectptr->x;
        temp.y += rectptr->y;
        self->srcrect = pgRect_New(&temp);
    }

    self->originx = temp.w / 2;
    self->originy = temp.h / 2;
    return 0;
}

static PyObject *
pg_image_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Uint8 rgba[] = { 255, 255, 255 };
    pgColorObject *col = (pgColorObject*) pgColor_NewLength(rgba, 3);
    if (!col) {
        return NULL;
    }
    pgImageObject *obj;
    obj = (pgImageObject*) type->tp_alloc(type, 0);
    obj->color = col;
    obj->alpha = 255;
    return (PyObject*)obj;
}

static void
pg_image_dealloc(pgImageObject *self) {} // TODO: doesn't need one?

static PyTypeObject pgImage_Type = {
    PyVarObject_HEAD_INIT(NULL,0)
    "pygame.Image", /*name*/
    sizeof(pgImageObject), /*basicsize*/
    0, /*itemsize*/
    (destructor)pg_image_dealloc, /*dealloc*/
    NULL, /*print*/
    NULL, /*getattr*/
    NULL, /*setattr*/
    NULL, /*compare/reserved*/
    NULL, /*repr*/
    NULL, /*as_number*/
    NULL, /*as_sequence*/
    NULL, /*as_mapping*/
    NULL, /*hash*/
    NULL, /*call*/
    NULL, /*str*/
    0L,
    0L,
    0L,
    Py_TPFLAGS_DEFAULT, /* tp_flags */
    NULL, /* TODO: docstring */
    NULL, /* tp_traverse */
    NULL, /* tp_clear */
    NULL, /* tp_richcompare */
    NULL, /* tp_weaklistoffset */
    NULL, /* tp_iter */
    NULL, /* tp_iternext */
    pg_image_methods, /* tp_methods */
    NULL, /* tp_members */
    pg_image_getset, /* tp_getset */
    NULL, /* tp_base */
    NULL, /* tp_dict */
    NULL, /* tp_descr_get */
    NULL, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)pg_image_init, /* tp_init */
    NULL, /* tp_alloc */
    pg_image_new /* tp_new */
};