import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

class Img:
    def __init__(self, index=None, kind=None, suffix=None):

        self.img = []
        self.masked = []

        if index != None and kind != None and suffix != None:
            img_prefix = './image fusion/test'
            self.img = np.array(Image.open('%s%s_%s.%s' % (img_prefix, index, kind, suffix)))

    def get_cut_pos(self):
        up, left, down, right = self.img.shape[0], self.img.shape[1], 0, 0

        for i in range(self.img.shape[0]):
            for j in range(self.img.shape[1]):
                if self.img[i, j, 0] > 0:
                    up = min(i, up)
                    down = max(i, down)
                    left = min(j, left)
                    right = max(j , right)

        return up, down, left, right

    def do_mask(self, mask, (x, y)):
        h, w = mask.img.shape[0], mask.img.shape[1]
        self.masked = self.img[x:x+h, y:y+w, :] * mask.img

    @staticmethod
    def get_gradient(img):
        grad_kern = np.array([[0, -1, 0], [-1, 4, -1], [0, -1, 0]])
        img_shape = img.shape
        tmp_shape = (img_shape[0] + 2, img_shape[1] + 2, img_shape[2])

        tmp_img = np.zeros(tmp_shape)
        for i in range(img_shape[0]):
            for j in range(img_shape[1]):
                tmp_img[i+1, j+1] = img[i, j]

        grad = np.zeros(img_shape)
        for i in range(grad.shape[0]):
            for j in range(grad.shape[1]):
                for k in range(grad.shape[2]):
                    grad[i, j, k] = np.sum(np.dot(grad_kern, tmp_img[i:i+3, j:j+3, k]))

        return grad

class Test:
    def __init__(self, index, suffix):
        self.index = index
        self.suffix = suffix

        self.src, none = self.get_img('src')
        self.target, none = self.get_img('target')

    def get_img(self, kind):
        res = Img(self.index, kind, self.suffix)
        up, down, left, right = (0, res.img.shape[0], 0, res.img.shape[1])

        if kind == 'mask':
            res.img = res.img / 200
            up, down, left, right = res.get_cut_pos()

        res.img = res.img[up:down, left:right, :]

        return res, (up, left)

    def fuse(self, x, y):
        mask, (mask_x, mask_y) = self.get_img('mask')
        mask_h, mask_w = mask.img.shape[0], mask.img.shape[1]
        self.src.do_mask(mask, (mask_x, mask_y))
        self.target.do_mask(mask, (x, y))

        for i in range(mask_h):
            for j in range(mask_w):
                if self.src.masked[i, j, 0] > 0:
                    self.target.img[i+x, y+j,:] = self.src.masked[i, j, :]

        target_grad = do_grad(self.target.masked)
        b = [target_grad[:,:,i].flatten('F') for i in range(3)]

test1 = Test(1, 'jpg')
test1.fuse(40, 70)
