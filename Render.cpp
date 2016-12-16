#include "Render.h"

namespace Render
{
	void RenderQueue::PushRenderObj(RenderObject* p, Camera& cam)
	{
		// TMP : hardcoded GatherRenderOps
		RenderOp rop = p->rop;
		rop.MVP = cam.projMat*cam.viewMat*p->frame.modelMatrix;
		rop.vao = p->glVao;
		rop.program = p->glProgram;
		rop.indCount = p->indCount;

		renderObjects.push_back(rop);
	}

	void RenderQueue::Reset()
	{
		renderObjects.clear();
	}

	void RenderQueue::Render()
	{
		for (auto & rop : renderObjects)
		{
			Execute(rop, 1);
		}
	}

	void Execute(RenderOp& rop, int count)
	{
		glUseProgram(rop.program);
		glUniformMatrix4fv(glGetUniformLocation(rop.program, "MVP"), 1, GL_FALSE, glm::value_ptr(rop.MVP));

		glBindVertexArray(rop.vao);
		glDrawElements(GL_TRIANGLES, rop.indCount, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		glUseProgram(0);
	}

	void RenderAll(Camera& cam, std::vector<RenderObject*> objects)
	{
		RenderQueue rq;

		for (auto& ro : objects)
		{
			if (cam.FrustumTest(ro))
			{
				rq.PushRenderObj(ro, cam);
			}
		}

		rq.Sort();
		rq.Render();

		rq.Reset();
	}
}